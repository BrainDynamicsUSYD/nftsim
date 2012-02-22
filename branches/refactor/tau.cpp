/***************************************************************************
                          tau.cpp  -  implements axonal time delay
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
using std::endl;
#include<cmath>
#include"tau.h"

void Tau::init( Configf& configf )
{
  m = configf.Numbers();
  if( m.size() == 1 ) {
    if( m[0] && fmod(m[0],deltat) ) {
      std::cerr<<"Value of tau not divisible by Deltat!"<<endl;
      exit(EXIT_FAILURE);
    }
    max = m[0];
  }
  else if( m.size() == uint(nodes) ) {
    for( int i=0; i<nodes; i++ ) {
      if( fmod(m[0],deltat) ) {
        std::cerr<<"Value of tau not divisible by Deltat!"<<endl;
        exit(EXIT_FAILURE);
      }
      if( m[i]>max )
        max = m[i];
    }
  }
  else {
    std::cerr << "The number of Tau has to be one or the same as the number of nodes." << endl;
    exit(EXIT_FAILURE);
  }
}

void Tau::restart( Restartf& restart )
{
}

void Tau::dump( Dumpf& dumpf ) const
{
  /*if( !m.empty() ){
    dumpf << "- TauabArray";
    for(long i=0;i<nodes;i++)
      dumpf << ": " << tauarr[i] << " ";
  }
  else
    dumpf << "- Tauab: " << tauab << " ";*/
};

Tau::Tau(void) : NF(0,0,0), max(0), m(1,0)
{
}

Tau::Tau( int nodes, double deltat, int index ) : NF(nodes,deltat,index), max(0)
{
}

Tau::~Tau()
{
}

void Tau::step(void)
{
}
