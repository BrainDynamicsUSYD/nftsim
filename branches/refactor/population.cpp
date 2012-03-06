/***************************************************************************
                          population.cpp  -  neural population object
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
using std::endl;
#include"population.h"

void Population::init( Configf& inputf )
{
  if( qresponse ) { // neural population
    double Qinit; inputf.Param("Q",Qinit);
    for( int i=0; i<3; i++ )
      qhistory.push_back( vector<double>(nodes,Qinit) );
    inputf.Param( "Firing", *qresponse );
  }
  else { // stimulus population
    for( int i=0; i<3; i++ )
      qhistory.push_back( vector<double>(nodes,0) );
    timeseries = new Timeseries(nodes,deltat,index);
    inputf.Param( "Stimulus", *timeseries );
  }
  settled = true;
}

void Population::restart( Restartf& restartf )
{
}

void Population::dump( Dumpf& dumpf ) const
{
}

Population::Population( int nodes, double deltat, int index )
    : NF(nodes,deltat,index),
	qkey(0), qresponse(0), timeseries(0), settled(false)
{
}

Population::~Population()
{
  if(qresponse) delete qresponse;
  if(timeseries) delete timeseries;
}

void Population::step(void)
{
  if( qresponse ) { // neural population
    qresponse->step();
    qresponse->fire( qhistory[qkey] );
  }
  else { // stimulus population
    for( int i=0; i<nodes; i++ )
      qhistory[qkey][i] = 0; // reset Q for stimulus
    timeseries->step();
    timeseries->fire( qhistory[qkey] );
  }
  // move pointer to keyring
  qkey = (qkey+1) % qhistory.size();
}

const vector<double>& Population::Q( const Tau& tau) const
{
  if( tau.m.size() == 1 )
    return qhistory
      [ tau.m[0]<=qkey ? qkey-tau.m[0] :qhistory.size()+qkey-tau.m[0] ];
  else { // tau.m.size() == nodes
    static vector<double> temp(nodes);
    for( int i=0; i<nodes; i++ )
      temp[i] = qhistory
        [ tau.m[i]<=qkey ? qhistory.size()+qkey-tau.m[i] : qkey-tau.m[i] ][i];
    return temp;
  }
}

const vector<double>& Population::V(void) const
{
  if( qresponse )
    return qresponse->V();
  else {
    std::cerr<<"Trying to access V of a stimulus population."<<endl;
    exit(EXIT_FAILURE);
  }
}

void Population::add2Dendrite( int index,
    Propag* const prepropag, Couple* const precouple )
{
  if( settled ) {
    std::cerr<<"Population is already settled, no more dendrites can be added!"
        <<endl;
    exit(EXIT_FAILURE);
  }

  if( !qresponse )
    qresponse = new QResponse(nodes,deltat,index);
  qresponse->add2Dendrite( index, prepropag, precouple );
}

void Population::growHistory( const Tau& tau )
{
  if( settled ) {
    std::cerr<<"Population is already settled, cannot resize firing history!"
        <<endl;
    exit(EXIT_FAILURE);
  }

  if( uint(tau.max) > qhistory.size() )
    qhistory.resize( tau.max, qhistory[0] );
}

vector<Output*> Population::output(void) const
{
  Output* temp = new Output( label("Pop.",index+1)+".V", V() );
  return vector<Output*>(1,temp);
}
