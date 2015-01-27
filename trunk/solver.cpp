#include"solver.h"
#include"wave.h"
#include"harmonic.h"
#include"longcouple.h"
#include"bcmlong.h"

using std::cerr;
using std::endl;
using std::stringstream;
using std::string;

void Solver::CntMat::init( Configf& configf )
{
  // Read the number of populations
  configf.next("From");
  // Expect format "From: 1 2 3 4", read the 
  vector<double> pop = configf.numbers();
  npop = pop.size();

  raw.resize(npop);

  for( int i=0; i<npop; i++ ) {
    configf.next( label("To ",i+1) ); // ignore "To ?:"
    raw[i] = configf.numbers();
    if( raw[i].size() != size_t(npop) ) {
      cerr << "The connection matrix is not configured correctly."
        << endl;
      exit(EXIT_FAILURE);
    }
  }

  // presynaptic population index for each connection index
  for( int i=0; i<npop; i++ )
    for( int j=0; j<npop; j++ )
      if( raw[i][j] )
        pre.push_back(j);

  // postsynaptic population index for each connection index
  for( int i=0; i<npop; i++ )
    for( int j=0; j<npop; j++ )
      if( raw[i][j] )
        post.push_back(i);

  ncnt = post.size(); // == pre.size()
}

Solver::Solver( Dumpf& dumpf )
    : NF(0,0,0), dumpf(dumpf), cnt()
{
}

Solver::~Solver()
{
  delete outputs;
}

void Solver::init( Configf& configf )
{
  // Anything before the first ':' is ignored as comment

  // read in simulation time and timestep
  double tempf; configf.param("Time",tempf);
  configf.param("Deltat",deltat);
  if( remainder(tempf,deltat) >deltat ) {
    cerr<<"Value of total simulation time not divisible by Deltat."<<endl;
    exit(EXIT_FAILURE);
  }
  else
    steps = tempf/deltat +.5;

  // read in grid size and grid geometry
  configf.param("Nodes",nodes);
  int longside;
  if( configf.optional("Longside",longside) ) {
    if( nodes%longside != 0 ) {
      cerr << "To define a rectangular grid nodes: " << nodes <<endl
        << "divided by Longside: " << longside << endl
        << "must have no remainder" << endl;
      exit(EXIT_FAILURE); 
    }
  }
  else
    longside = sqrt(nodes);
  string topology("Torus");
  if( configf.optional("Topology",topology) )
    if( topology == "Nonperiodic" ) {
      double bath;
      configf.param("Bath",bath);
      stringstream ss; ss<<topology<<" "<<bath;
      topology = ss.str();
    }

  // read in connection matrix
  configf.param("Connection matrix",cnt);

  // construct populations
  for( int i=0; i<cnt.npop; i++ ) /*{
    bool neuralpop = false; // marker of neural or Betz population
    for( int j=0; j<cnt.ncnt; j++ )
      if( cnt.pre[j] == i )
        neuralpop = true; // a Betz cell is one without postsynaptic connections
    if( neuralpop ) // neural population includes stimulus populations too*/
      pops.add( new Population(nodes,deltat,i) );
    /*else // Betz cell
      pops.add( new Single(nodes,deltat,i) );
  }*/

  for( int i=0; i<cnt.ncnt; i++ ) {
    string ptype = configf.find( label("Propag ",i+1) +":" );
    // PUT YOUR PROPAGATORS HERE
    if(ptype=="Map")
      propags.add( new
        Propag(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
    else if(ptype=="Wave") {
      if( nodes==1 )
      propags.add( new
        Harmonic(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
      else
      propags.add( new
        Wave(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
    }
    else if(ptype=="Harmonic")
      propags.add( new
        Harmonic(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
    else {
      cerr<<"Invalid propagator type '"<<ptype<<"'."<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR PROPAGATORS HERE

    string ctype = configf.find( label("Couple ",i+1) +":" );
    // PUT YOUR COUPLES HERE
    if(ctype=="Map")
      couples.add( new
        Couple(nodes,deltat,i, *propags[i], *pops[cnt.post[i]] ) );
    else if(ctype=="Matrix")
      couples.add( new
        LongCouple(nodes,deltat,i, *propags[i], *pops[cnt.post[i]] ) );
    else if(ctype=="CaDP")
      couples.add( new
        CaDP(nodes,deltat,i, *propags[i], *pops[cnt.post[i]] ) );
    else if(ctype=="BCM")
      couples.add( new
        BCM(nodes,deltat,i, *propags[i], *pops[cnt.post[i]] ) );
    else if(ctype=="BCM-Spatial")
      couples.add( new
        BCMLong(nodes,deltat,i, *propags[i], *pops[cnt.post[i]] ) );
    //else if(ctype=="fCaP")
      //couples.add( new
        //fCaP(nodes,deltat,i, *propags[i], *pops[cnt.post[i]] ) );
    //else if(ctype=="Epilepsy")
      //couples.add( new
        //Epilepsy(nodes,deltat,i, *propags[i], *pops[cnt.post[i]] ) );
    else {
      cerr<<"Invalid couple type '"<<ctype<<"'."<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR COUPLES HERE
  }

  // insert dendrites to each population
  for( int i=0; i<cnt.npop; i++ )
    for( int j=0; j<cnt.ncnt; j++ )
      if( cnt.post[j] == i )
        pops[i]->add2Dendrite( j, *propags[j], *couples[j], configf );

  // read object parameters
  for( int i=0; i<cnt.npop; i++ )
    configf.param( label("Population ",i+1), *pops[i] );
  for( int i=0; i<cnt.ncnt; i++ )
    configf.param( label("Propag ",i+1), *propags[i] );
  for( int i=0; i<cnt.ncnt; i++ )
    configf.param( label("Couple ",i+1), *couples[i] );

  // initialize outputs
  configf.go2("Output"); configf.next("Output");
  outputs = new Outputs(nodes,deltat,dumpf,cnt,pops,propags,couples);
  outputs->init(configf);
}

void Solver::solve(void)
{
  for( int i=0; i<steps; i++ )
    step();
}

void Solver::step(void)
{
  // step through populations
  couples.pstep();
  pops.pstep();
  propags.pstep();
  outputs->step();
}

void Solver::Outputs::step(void)
{
  if( outputstart )
    outputstart--;
  else {
    t++;
    if( t%outputinterval==0 ) {
      dumpf<<double(t)*deltat<<space<<space<<septor;
      for( size_t i=0; i<outlets.size(); i++ )
        writeOutlet( *outlets[i] );
      dumpf<<endl;
    }
  }
}
void Solver::Outputs::init( Configf& configf )
{
  double tempf; string temps; vector<double> temp_node;
  // read in nodes to output
  //int position = tellg();
  if( configf.next("Node") )
    temp_node = configf.numbers();
  if( temp_node.empty() )
  {
    //seekg(position,ios::beg);
    for( int i=0; i<nodes; i++ )
      node.push_back(i);
  }

  for( size_t i=0; i<temp_node.size(); i++ )
    if( temp_node[i] > nodes ) {
      cerr<<"Trying to plot node number "<<temp_node[i]
          <<", which is bigger than the highest node index."<<endl;
      exit(EXIT_FAILURE);
    }
    else
      node.push_back( temp_node[i]-1 );

  // read in time to start of output
  if( !configf.optional("Start",tempf) )
    outputstart = 0;
  else {
    if( remainder(tempf,deltat) >deltat ) {
      cerr<<"Value of output start time not divisible by Deltat."<<endl;
      exit(EXIT_FAILURE);
    }
    else
      outputstart = tempf/deltat;
  }
  t = 0;

  // read in output interval
  if( !configf.optional("Interval",tempf) )
    outputinterval = 1;
  else {
    if( remainder(tempf,deltat) >deltat ) {
      cerr<<"Value of output interval not divisible by Deltat."<<endl;
      exit(EXIT_FAILURE);
    }
    else
      outputinterval = tempf/deltat+.5;
  }

  // read in populations to output
  configf.next("Population");
  vector<string> temp = configf.arb("Dendrite:");
  for( size_t i=0; i<temp.size(); i++ ) {
    int obj_index = atoi(temp[i].c_str()); // atoi() takes only 1 of "1.V"
    if( obj_index > cnt.npop || obj_index<1 ) {
      cerr<<"Trying to output population "<<obj_index
          <<", which is an invalid population."<<endl;
      exit(EXIT_FAILURE);
    }
    string key;
    if( temp[i].find(".")!=string::npos )
      key = temp[i].substr( temp[i].find(".")+1, string::npos );
    Output output(key);
    pops[obj_index-1]->output(output);
    if( output.empty() ) {
      cerr<<"Population "<<temp[i].c_str()<<" cannot be outputted."<<endl;
      exit(EXIT_FAILURE);
    }
    add(output);
  }

  // read in dendrites to output
  configf.next("Dendrite");
  temp = configf.arb("Propag:");
  for( size_t i=0; i<temp.size(); i++ ) {
    int obj_index = atoi(temp[i].c_str());
    if( obj_index > cnt.ncnt || obj_index<1 ) {
      cerr<<"Trying to output dendrite "<<obj_index
          <<", which is an invalid dendrite."<<endl;
      exit(EXIT_FAILURE);
    }
    string key;
    if( temp[i].find(".")!=string::npos )
      key = temp[i].substr( temp[i].find(".")+1, string::npos );
    Output output(key);
    for( size_t j=0; j<pops.size(); j++ )
      pops[j]->outputDendrite(obj_index-1,output);
    if( output.empty() ) {
      cerr<<"Dendrite "<<temp[i].c_str()<<" cannot be outputted."<<endl;
      exit(EXIT_FAILURE);
    }
    add(output);
  }

  // read in propags to output
  configf.next("Propag");
  temp = configf.arb("Couple:");
  for( size_t i=0; i<temp.size(); i++ ) {
    int obj_index = atoi(temp[i].c_str());
    if( obj_index > cnt.ncnt || obj_index<1 ) {
      cerr<<"Trying to output propag "<<obj_index
          <<", which is an invalid propag."<<endl;
      exit(EXIT_FAILURE);
    }
    string key;
    if( temp[i].find(".")!=string::npos )
      key = temp[i].substr( temp[i].find(".")+1, string::npos );
    Output output(key);
    propags[obj_index-1]->output(output);
    if( output.empty() ) {
      cerr<<"Propag "<<temp[i].c_str()<<" cannot be outputted."<<endl;
      exit(EXIT_FAILURE);
    }
    add(output);
  }

  // read in couples to output
  configf.next("Couple");
  temp = configf.arb("EOF"); // config files do not contain "EOF", reads to end of file
  for( size_t i=0; i<temp.size(); i++ ) {
    int obj_index = atoi(temp[i].c_str());
    if( obj_index > cnt.ncnt || obj_index<1 ) {
      cerr<<"Trying to output couple "<<obj_index
          <<", which is an invalid couple."<<endl;
      exit(EXIT_FAILURE);
    }
    string key;
    if( temp[i].find(".")!=string::npos )
      key = temp[i].substr( temp[i].find(".")+1, string::npos );
    Output output(key);
    couples[obj_index-1]->output(output);
    if( output.empty() ) {
      cerr<<"Couple "<<temp[i].c_str()<<" cannot be outputted."<<endl;
      exit(EXIT_FAILURE);
    }
    add(output);
  }

  // write out first row
  dumpf<<space<<"Time"<<space<<space<<septor;
  for( size_t i=0; i<outlets.size(); i++ )
    writeName(*outlets[i]);
  // write out second row
  dumpf<<endl<<space<<space<<space<<" "<<septor;
  for( size_t i=0; i<outlets.size(); i++ )
    writeNode(*outlets[i]);
  dumpf<<endl;
}

void Solver::Outputs::writeName( Outlet& outlet )
{
  if( outlet.single_output )
    dumpf<<space<<space<<setw<<outlet.name;
  else {
    for( size_t i=0; i<node.size(); i++ )
      dumpf<<space<<space<<setw<<outlet.name;
  }
  dumpf<<space<<space<<septor;
}

void Solver::Outputs::writeNode( Outlet& outlet )
{
  if( outlet.single_output )
    dumpf<<space<<space<<setw<<1;
  else {
    for( size_t i=0; i<node.size(); i++ )
      dumpf<<space<<space<<setw<<int(node[i]+1);
  }
  dumpf<<space<<space<<septor;
}

void Solver::Outputs::writeOutlet( Outlet& outlet )
{
  if( outlet.single_output )
    dumpf<<space<<outlet.field[0]<<space<<space<<septor;
  else {
    for( size_t i=0; i<node.size(); i++ )
      dumpf<<space<<outlet.field[node[i]];
    dumpf<<space<<space<<septor;
  }
}
