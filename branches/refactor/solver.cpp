/***************************************************************************
                          solver.cpp  -  runs the NeuroField simulation
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cmath>
#include<string>
using std::string;
#include<iostream>
using std::endl;

#include"solver.h"
#include"population.h"
#include"array.h"
#include"configf.h"

#include"propag.h"
//#include"waveeqn.h"
//#include"pmap.h"
//#include"eqnset.h"
//#include"pharmonic.h"

#include"couple.h"
#include"cadp.h"

void Solver::CntMat::init( Configf& configf )
{
  // Read the number of populations
  configf.Next("From");
  // Expect format "From: 1 2 3 4", read the 
  vector<double> pop = configf.Numbers();
  npop = pop.size();

  raw.resize(npop);

  for( int i=0; i<npop; i++ ) {
    configf.Next( label("To ",i+1) ); // ignore "To ?:"
    raw[i] = configf.Numbers();
    if( raw[i].size() != uint(npop) ) {
      std::cerr << "The connection matrix is not configured correctly."
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

Solver::Solver(Dumpf* dumpf)
    : NF(0,0,0), dumpf(dumpf)
{
}

Solver::~Solver()
{
  if(dumpf)
    *dumpf<<*this;
}

void Solver::init( Configf& configf )
{
  // Parse in global parameters from conf file
  // Anything before the first ':' is ignored as comment

  configf.Param("Integration steps",steps);
  if( !configf.Optional("Skip steps",skip) ) skip = 0;
  configf.Param("Deltat",deltat);

  configf.Param("Nodes",nodes);
  int longside;
  if( configf.Optional("Longside",longside) ) {
    if( nodes%longside != 0 ) {
      std::cerr << "To define a rectangular grid nodes: " << nodes <<endl
        << "divided by Longside: " << longside << endl
        << "must have no remainder" << endl;
      exit(EXIT_FAILURE); 
    }
  }
  else
    longside = sqrt(nodes);

  // glutamte dynamics
  glu.resize(nodes,0); dglu.resize(nodes,0);
  configf.Param("Lambda",Lambda); configf.Param("tGlu",tGlu);

  // read in connection matrix
  configf.Next("Connection matrix"); cnt.init(configf);

  // construct populations
  for( int i=0; i<cnt.npop; i++ )
    pops.add( new Population(nodes,deltat,i) );

  for( int i=0; i<cnt.ncnt; i++ ) {
    string ptype = configf.Find( label("Propag ",i+1) +":" );
    // PUT YOUR PROPAGATORS HERE
    if(ptype=="Map")
      propags.add( new
        Propag(nodes,deltat,index, pops[cnt.pre[i]], pops[cnt.post[i]], longside));
    /*else if(ptype=="Wave")
      propags.add( new
        Wave(nodes,deltat, pops[cnt.pre[i]], pops[cnt.post[i]], longside));
    else if(ptype=="Eqnset")
      propags.add( ne
        Eqnset(nodes,deltat, pops[cnt.pre[i]], pops[cnt.post[i]], longside));
    else if(ptype=="Harmonic")
      propags.add( new
        PHarmonic(nodes,deltat, pops[cnt.pre[i]], pops[cnt.post[i]], longside));*/
    else {
      std::cerr<<"Invalid propagator type '"<<ptype<<"'."<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR PROPAGATORS HERE

    string ctype = configf.Find( label("Couple ",i+1) +":" );
    // PUT YOUR COUPLES HERE
    if(ctype=="Map")
      couples.add( new
        Couple(nodes,deltat,index, glu, pops[cnt.pre[i]], pops[cnt.post[i]] ) );
    else if(ctype=="Calcium")
      couples.add( new
        CaDP(nodes,deltat,index, glu, pops[cnt.pre[i]], pops[cnt.post[i]] ) );
    else {
      std::cerr<<"Invalid couple type '"<<ctype<<"'."<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR COUPLES HERE
  }

  // insert dendrites to each population
  for( int i=0; i<cnt.npop; i++ )
    for( int j=0; j<cnt.ncnt; j++ )
      if( cnt.post[j] == i )
        pops[i]->add2Dendrite( j, propags[j], couples[j] );

  // read populations parameters
  for( int i=0; i<cnt.npop; i++ )
    configf.Param( label("Population ",i+1), *pops[i] );

  // read propags parameters
  for( int i=0; i<cnt.ncnt; i++ )
    configf.Param( label("Propag ",i+1), *propags[i] );

  // read couples parameters
  for( int i=0; i<cnt.ncnt; i++ )
    configf.Param( label("Couple ",i+1), *couples[i] );

  // initialize outputfs
  /*configf.Next("Outputs"); configf.Next("Nodes");
  vector<double> temp = configf.Numbers();
  for( uint i=0; i<temp.size(); i++ )
    if( temp[i] >= nodes ) {
      std::cerr<<"Trying to plot node number "<<temp[i]
          <<", which is bigger than the highest node index."<<endl;
      exit(EXIT_FAILURE);
    }
    else
      Outputf::node[i] = temp[i];
  while( configf.Next("Population") ) {
    temp = configf.Numbers();
    for( uint i=0; i<temp.size(); i++ ) {
      if( temp[i] >= cnt.npop ) {
        std::cerr<<"Trying to print population "<<i
            <<", which is an invalid population."<<endl;
        exit(EXIT_FAILURE);
      }
      pops[i]->output(outputfs);
    }
  }
  while( configf.Next("Propag") ) {
    temp = configf.Numbers();
    for( uint i=0; i<temp.size(); i++ ) {
      if( temp[i] >= cnt.ncnt ) {
        std::cerr<<"Trying to print propagator "<<i
            <<", which is an invalid propagator."<<endl;
        exit(EXIT_FAILURE);
      }
      propags[i]->output(outputfs);
    }
  }
  while( configf.Next("Couple") ) {
    temp = configf.Numbers();
    for( uint i=0; i<temp.size(); i++ ) {
      if( temp[i] >= cnt.ncnt ) {
        std::cerr<<"Trying to print couple "<<i
            <<", which is an invalid couple."<<endl;
        exit(EXIT_FAILURE);
      }
      couples[i]->output(outputfs);
    }
  }*/
}

void Solver::restart( Restartf& restartf )
{
}

void Solver::dump(std::ofstream& dumpf) const
{
  dumpf << pops;
  dumpf << propags;
  dumpf << couples;
  dumpf << outputfs;
}

void Solver::solve(void)
{	std::cout.precision(14);
std::cout<<std::scientific;
  for( int i=0; i<steps; i++ ) {
    step();
    std::cout<<pops[1]->V()[0]<<"\t";
	std::cout<<couples[1]->nu()[0]*4200<<"\t|\t";
    std::cout<<propags[0]->phi()[0]<<"\t"<<propags[1]->phi()[0]<<endl;
    if( skip==0 )
      outputfs.step();
    else
      skip--;
  }
}

void Solver::step(void)
{
  // glutamte dynamics
  for( int j=0; j<nodes; j++ )
    dglu[j] = 0;//double ddglu = 0; double ts = 200e-3; double td = 200e-3;
  for( uint i=0; i<couples.size(); i++ )
    if( couples[i]->excite() )
      for( int j=0; j<nodes; j++ )
        dglu[j] += Lambda*propags[i]->phi()[j]*deltat;
  for( int j=0; j<nodes; j++ ) {
    dglu[j] -= glu[j]/tGlu*deltat;
    glu[j] += dglu[j];
    if( glu[j]<0 ) glu[j] = 0;
    /*double p1 = ddglu;
      double k1 = dglu[j];
      double p2 = p1 -p1/2/ts -k1/2/td;
      double k2 = dglu[j] +1/2*(dglu[j]+p1);
      double p3 = p1 -p2/2/ts -k2/2/td;
      double k3 = dglu[j] +1/2*(dglu[j]+p2);
      double p4 = p1 -p3/ts -k3/td;
      double k4 = dglu[j] +dglu[j]+p3;
      dglu[j] += 1/6*(p1+2*p2+2*p3+p4);
      glu[j] += 1/6*(k1+2*k2+2*k3+k4);
    */
  }

  pops.step();
  propags.step();
  couples.step();
}
