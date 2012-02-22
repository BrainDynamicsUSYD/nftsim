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
    qhistory.resize( 1, vector<double>(nodes,Qinit) );
    inputf.Param( "Firing", *qresponse );
  }
  else { // stimulus population
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
    timeseries->step();
    for( int i=0; i<nodes; i++ )
      qhistory[qkey][i] = 0; // reset Q for stimulus
    timeseries->fire( qhistory[qkey] );
  }
  // move pointer to keyring
  qkey++;
  if( uint(qkey) == qhistory.size() )
    qkey = 0;
}

const vector<double>& Population::Q( const Tau& tau) const
{
  if( tau.m.size() == 1 ) {
    double n = tau.m[0];
    return qhistory[ n<qkey ? qhistory.size()+n-qkey : n-qkey ];
  }
  else { // tau.m.size() == nodes
    static vector<double> temp(nodes);
    for( int i=0; i<nodes; i++ )
      temp[i] = qhistory[
        tau.m[i]<qkey ? qhistory.size()+tau.m[i]-qkey : tau.m[i]-qkey ][i];
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
