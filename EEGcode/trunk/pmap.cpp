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
  inputf.validate("Tauab",58);
  inputf >> tauab;
}

void Pmap::dump(ofstream& dumpf){
  dumpf << "Tau_ab: " << tauab << " ";
  dumpf << endl;
}

void Pmap::restart(Istrm& restartf){
  restartf.ignore(200,58); // throw away everything before the colon character
  restartf >> tauab;
}

void Pmap::stepwaveeq(double *Phi, Qhistory *pqhistory){
  double* Q= pqhistory->getQbytime(tauab);
  for(long i=0; i<nodes; i++){
      Phi[i]=Q[i];
  }
}
