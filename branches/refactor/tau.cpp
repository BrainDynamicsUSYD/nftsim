/***************************************************************************
                          tau.cpp  -  time delay object
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cstdlib>
#include "tau.h"
#include "qhistory.h"
using std::endl;

Tau::Tau(long nodes,double dt,Istrm& inputf,Qhistory& qhistory):
            nodes(nodes), deltat(dt) {
  int taumax=0; // maximum timesteps amongst the tau values
  float tauabfloat;
  if( inputf.Optional("Tauab",tauabfloat) ) {
    tauab = int(tauabfloat);
    taumax = tauab;
  }
  else if( inputf.Optional("Tauab",tauabfloat) ) {
    tauabfloat /= deltat;
    tauab = int(tauabfloat);
    taumax = tauab;
  }
  else if( inputf.Optional("TauabArray",tauabfloat) ) {
    qarray = new double[nodes];
    tauarr[0] = int(tauabfloat);
    taumax = tauarr[0];
    tauarr = inputf.Numbers();
    if( tauarr.size() != unsigned(nodes) ) {
      std::cerr << "The number of Tauab has to be the same as the number of nodes." << endl;
      exit(EXIT_FAILURE);
    }
    for( int i=1; i<nodes; i++ )
      if( tauarr[i] > taumax )
        taumax = tauarr[i];
  }
  else if( inputf.Optional("TauabArrayt",tauabfloat) ) {
    qarray = new double[nodes];
    tauabfloat /= deltat;
    tauarr[0] = int(tauabfloat);
    taumax = tauarr[0];
    tauarr = inputf.Numbers();
    if( tauarr.size() != unsigned(nodes) ) {
      std::cerr << "The number of Tauab has to be the same as the number of nodes." << endl;
      exit(EXIT_FAILURE);
    }
    for( int i=1; i<nodes; i++ ) {
      tauarr[i] /= deltat;
      if( tauarr[i] > taumax )
        taumax = tauarr[i];
    }
  }
  else {
    std::cerr << "Last read looking for Tauab, Tauabt, TauabArray, or";
    std::cerr << " TauabtArray found none of them." << endl;
    exit(EXIT_FAILURE);
  }
  if( 0<tauabfloat && tauabfloat<1 ) {
    std::cerr << "Last read Tauab: " << tauabfloat << endl;
    std::cerr << "Tauab must be greater than 1 as it is measured in" << endl;
    std::cerr << "time steps not a time measured in seconds." << endl;
    exit(EXIT_FAILURE);
  }

  qhistory.grow(taumax);
}

Tau::~Tau()
{
}

void Tau::dump(std::ofstream& dumpf){
  if( tauarr.empty() ){
    dumpf << "- TauabArray";
    for(long i=0;i<nodes;i++)
      dumpf << ": " << tauarr[i] << " ";
  }
  else
    dumpf << "- Tauab: " << tauab << " ";
};
