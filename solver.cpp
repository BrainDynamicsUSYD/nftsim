#include<cmath>
#include<string>
using std::string;
#include<iostream>
using std::cerr;
using std::endl;
#include<sstream>
using std::stringstream;

#include"solver.h"
#include"population.h"
#include"single.h"
#include"array.h"
#include"configf.h"

#include"propag.h"
#include"wave.h"
#include"harmonic.h"
#ifdef WAVEFOURIER
#include"wavefourier.h"
#endif

#include"couple.h"
#include"cadp.h"
#include"fcap.h"
#include"stp.h"

#include"output.h"

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

void Solver::CntMat::dump( Dumpf& dumpf ) const
{
  dumpf << "Connection matrix:" << endl
    << "From:";
  for( int i=0; i<npop; i++ )
    dumpf << " " << i;
  for( int i=0; i<npop; i++ ) {
    dumpf << endl << "To " << i+1 << ":";
    for( int j=0; j<npop; j++ )
      dumpf << " " << raw[i][j];
  }
  dumpf << endl << endl;
}

Solver::Solver( Dumpf* dumpf )
    : NF(0,0,0), dumpf(dumpf), cnt(0,0,0)
{
}

Solver::~Solver()
{
  if(dumpf) {
    *dumpf<<*this;
    delete dumpf;
  }
  delete glu; delete glu_rk4;
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

  // read in glutamate dynamics
  glu = new Glu(nodes,deltat); glu_rk4 = new RK4(*glu);
  glu->init(configf);

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
    #ifdef WAVEFOURIER
    else if(ptype=="WaveFourier") {
      if( nodes==1 )
      propags.add( new
        Harmonic(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
      else
      propags.add( new
        WaveFourier(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
    }
    #endif
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
        Couple(nodes,deltat,i,(*glu)[0], *propags[i], *pops[cnt.post[i]] ) );
    else if(ctype=="CaDP")
      couples.add( new
        CaDP(nodes,deltat,i,(*glu)[0], *propags[i], *pops[cnt.post[i]] ) );
    else if(ctype=="fCaP")
      couples.add( new
        fCaP(nodes,deltat,i,(*glu)[0], *propags[i], *pops[cnt.post[i]] ) );
    else if(ctype=="STP")
      couples.add( new
        STP(nodes,deltat,i,(*glu)[0], *propags[i], *pops[cnt.post[i]] ) );
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

  // read couples parameters
  configf.go2("Couple 1");
  for( int i=0; i<cnt.ncnt; i++ )
    configf.param( label("Couple ",i+1), *couples[i] );

  // read propags parameters
  configf.go2("Propag 1");
  for( int i=0; i<cnt.ncnt; i++ )
    configf.param( label("Propag ",i+1), *propags[i] );

  // read populations parameters
  configf.go2("Population 1");
  for( int i=0; i<cnt.npop; i++ )
    configf.param( label("Population ",i+1), *pops[i] );

  // initialize outputs
  configf.go2("Output"); configf.next("Output");
  initOutput(configf);
}

void Solver::initOutput( Configf& configf )
{
  double tempf;
  // read in nodes to output
  configf.next("Node");
  if( configf.find("Node:") == "All" ) // beware of this slightly hackish line
    for( int i=0; i<nodes; i++ )
      Outlet::node.push_back(i);
  else {
    vector<double> temp = configf.numbers();
    for( size_t i=0; i<temp.size(); i++ )
      if( temp[i] > nodes ) {
        cerr<<"Trying to plot node number "<<temp[i]
            <<", which is bigger than the highest node index."<<endl;
        exit(EXIT_FAILURE);
      }
      else
        Outlet::node.push_back( temp[i]-1 );
  }

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
      cerr<<"Output "<<temp[i].c_str()<<" cannot be outputted."<<endl;
      exit(EXIT_FAILURE);
    }
    outputs.add(output);
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
      cerr<<"Output "<<temp[i].c_str()<<" cannot be outputted."<<endl;
      exit(EXIT_FAILURE);
    }
    outputs.add(output);
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
      cerr<<"Output "<<temp[i].c_str()<<" cannot be outputted."<<endl;
      exit(EXIT_FAILURE);
    }
    outputs.add(output);
  }

  // read in couples to output
  configf.next("Couple");
  temp = configf.arb("Nonexistent:");
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
      cerr<<"Output "<<temp[i].c_str()<<" cannot be outputted."<<endl;
      exit(EXIT_FAILURE);
    }
    outputs.add(output);
  }

  // write out first row
  Outlet::dumpf<<space<<"Time"<<space<<space<<septor;
  for( size_t i=0; i<outputs.size(); i++ ) {
    outputs[i]->writeName();
  }
  // write out second row
  Outlet::dumpf<<endl<<space<<space<<space<<" "<<septor;
  for( size_t i=0; i<outputs.size(); i++ ) {
    outputs[i]->writeNode();
  }
  Outlet::dumpf<<endl;
}

void Solver::restart( Restartf& restartf )
{
}

void Solver::dump( Dumpf& dumpf ) const
{
}

void Solver::solve(void)
{
  for( int i=0; i<steps; i++ )
    step();
}

void Solver::Glu::init( Configf& configf )
{
  configf.optional("fast Lambda",fLambda); configf.optional("fast Glu",tfGlu);
  configf.optional("slow Lambda",sLambda); configf.optional("slow Glu",tsGlu);
  variables[0].resize(nodes,1e-4); variables[1].resize(nodes);
}

void Solver::Glu::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y = { glu, excitatory phi }
  // glu
  dydt[0] = fLambda*y[1] -y[0]/tsGlu;
  if( y[0]+dydt[0]*deltat <0 ) dydt[0] = -y[0];
  // excitatory phi, leave alone
  dydt[1] = 0;
}

void Solver::step(void)
{
  // glutamte dynamics
  if( glu->fLambda != 0 ) {
    for( int j=0; j<nodes; j++ )
      (*glu)[1][j] = 0; // reset excitatory phi
    for( size_t i=0; i<couples.size(); i++ )
      if( couples[i]->excite() )
        for( int j=0; j<nodes; j++ )
          (*glu)[1][j] += (*propags[i])[j]; // put in excitatory phi
    glu_rk4->step();
  }

  // step through populations
  couples.step();
  pops.step();
  propags.step();

  // output routine
  if( outputstart )
    outputstart--;
  else {
    t++;
    if( t%outputinterval==0 ) {
      Outlet::dumpf<<double(t)*deltat<<space<<space<<septor;
      outputs.step();
      Outlet::dumpf<<endl;
    }
  }
}


