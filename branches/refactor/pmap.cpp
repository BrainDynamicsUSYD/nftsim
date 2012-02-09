/***************************************************************************
                          pmap.cpp  -  Propagator which maps \phi(t)=Q(t-\tau)
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include"pmap.h"
#include<math.h>
#include"qhistory.h"
using std::endl;

Pmap::Pmap(long nodes, double dt):nodes(nodes),deltat(dt){
}

Pmap::~Pmap(){
  if(tauobj) delete tauobj;
}

void Pmap::init(Istrm& inputf,Qhistory& qhistory){
  inputf.ignore(45); // Throw away everything up to the dash char
  tauobj= new Tau(nodes,deltat,inputf,qhistory);
}

void Pmap::dump(std::ofstream& dumpf){
  tauobj->dump(dumpf);
}

void Pmap::restart(Istrm& restartf,Qhistory& qhistory){
  init(restartf,qhistory);
}

void Pmap::stepwaveeq(double * __restrict__ Phi,Qhistory& qhistory){
  double* __restrict__ Q=qhistory.getQbytime(*tauobj);
  long n=nodes;
  for(long i=0; i<n; i++){
      Phi[i]=Q[i];
  }
}
