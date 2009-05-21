/***************************************************************************
                          pmap.cpp  -  Propagator which maps \phi(t)=Q(t-\tau)
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "pmap.h"
#include<math.h>

Pmap::Pmap(long totalnodes, double dt):nodes(totalnodes),deltat(dt){
}

Pmap::~Pmap(){
}

void Pmap::init(Istrm& inputf, Qhistory* qhistory){
  inputf.ignore(200,45); // Throw away everything up to the dash char
  tauab=inputf.readtauab(deltat);
}

void Pmap::dump(ofstream& dumpf){
  dumpf << "- Tauab: " << tauab << " " << endl;
}

void Pmap::restart(Istrm& restartf){
  Qhistory* dummy;
  init(restartf,dummy);
}

void Pmap::stepwaveeq(double * __restrict__ Phi, Qhistory *pqhistory){
  double* __restrict__ Q= pqhistory->getQbytime(tauab);
  long n=nodes;
  for(long i=0; i<n; i++){
      Phi[i]=Q[i];
  }
}
