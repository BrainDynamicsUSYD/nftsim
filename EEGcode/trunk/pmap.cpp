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
  int optionnum;
  optionnum=inputf.choose("Tauab:1 Tauabt:2",58);
  float tauabfloat;
  if(1==optionnum){
    inputf >> tauabfloat;
  }
  if(2==optionnum){
    double tauabt;
    inputf >> tauabt;
    tauabfloat=tauabt/deltat;
  }
  if( !((1==optionnum)||(2==optionnum)) ){
    cerr << "Last read looking for Tauab or Taubt found neither" << endl;
    exit(EXIT_FAILURE);
  }
  tauab=int(tauabfloat);
  if(tauabfloat<1 && tauabfloat>0){
    cerr << "Last read Tauab: " << tauabfloat << endl;
    cerr << "Tauab must be greater than 1 as it is measured in" << endl;
    cerr << "time steps not a time measured in seconds" << endl;
    exit(EXIT_FAILURE);
  }
}

void Pmap::dump(ofstream& dumpf){
  dumpf << "- Tauab: " << tauab << " ";
  dumpf << endl;
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
