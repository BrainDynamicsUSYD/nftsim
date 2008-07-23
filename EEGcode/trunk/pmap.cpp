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
  float tauabfloat;
  inputf >> tauabfloat;
  tauab=int(tauabfloat);
  if(tauabfloat<1 && tauabfloat>0){
    cerr << "Last read Tauab: " << tauabfloat << endl;
    cerr << "Tauab must be greater than 1 as it is measured in" << endl;
    cerr << "time steps not a time measured in seconds" << endl;
    exit(EXIT_FAILURE);
  }
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
