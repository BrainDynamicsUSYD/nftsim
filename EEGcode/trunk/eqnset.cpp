/***************************************************************************
                          eqnset.cpp  -  Propagator for wave equation with
                                         K extention
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "eqnset.h"
#include "prefact.h"
#include<math.h>

Eqnset::Eqnset(long gsize, double deltat):
               gridsize(gsize){
  rowlength=static_cast<long>(sqrt(gridsize));
  sidelength=rowlength-2;
  startfirstrow=rowlength+1;
  totalnodes=sidelength*sidelength;
  uRe = new double[totalnodes];
  uIm = new double[totalnodes];
  weqnobj = new Weqn(gridsize, deltat);
}

Eqnset::~Eqnset(){
  delete[] uRe;
  delete[] uIm;
  delete weqnobj;
  for(int i=0; i<numk; i++){
    if (karray[i]) delete [ ] karray[i];
  }
  if (karray) delete [ ] karray;
  for(int i=0; i<numk; i++){
    if (field[i]) delete field[i];
    if (scalfactarr[i]) delete scalfactarr[i];
  }
  if (field) delete [ ] field;
  if (scalfactarr) delete [ ] scalfactarr;
}

void Eqnset::init(Istrm& inputf){
  inputf.validate("Deltax",58);
  inputf >> deltax;
  weqnobj->init(inputf,deltax);
  inputf.validate("Geometric centre",58);
  inputf >> centrex;
  inputf.validate("",58);
  inputf >> centrey;
  inputf.validate("Num. K",58);
  inputf >> numk;
  karray= new double *[numk];
  double ktmp;
  for(int i=0; i<numk; i++){
    karray[i]= new double[2];
    double * kvect;
    kvect=karray[i];
    inputf.validate("",58);
    inputf >> ktmp;
    kvect[0]=ktmp;
    inputf.validate("",44);
    inputf >> ktmp;
    kvect[1]=ktmp;
  }
  field = new Wavefield *[numk];
  scalfactarr = new Prefact *[numk];
  for(int i=0; i<numk; i++){
    field[i]= new Wavefield(gridsize);
    field[i]->init(inputf);
    scalfactarr[i]= new Prefact(gridsize);
    scalfactarr[i]->precalcfact(karray[i], deltax, centrex, centrey);
  }
}

void Eqnset::dump(ofstream& dumpf){
  dumpf << "Deltax:" << deltax << " ";
  weqnobj->dump(dumpf);
  dumpf << "geometric centre:";
  dumpf << centrex << " :" << centrey << " ";
//
// Next lines dump array of k values
//
  dumpf << "Num. K components:";
  dumpf << numk << endl;
  double* kvect;
  for(int i=0; i<numk; i++){
    kvect=karray[i];
    dumpf << ":" << kvect[0] << "," << kvect[1] << " ";
  }
//
// Dump wavefields
//
  for(int i=0; i<numk; i++)
    field[i]->dump(dumpf);
}

void Eqnset::restart(Istrm& restartf){
  restartf.validate("Deltax",58);
  restartf >> deltax;
  weqnobj->restart(restartf,deltax);
  restartf.validate("geometric centre",58);
  restartf >> centrex;
  restartf.validate("",58);
  restartf >> centrey;
  restartf.validate("Num. K components",58);
  restartf >> numk;
  karray= new double *[numk];
  double ktmp;
  for(int i=0; i<numk; i++){
    karray[i]= new double[2];
    double * kvect;
    kvect=karray[i];
    restartf.validate("",58);
    restartf >> ktmp;
    kvect[0]=ktmp;
    restartf.validate("",44);
    restartf >> ktmp;
    kvect[1]=ktmp;
  }
  field = new Wavefield *[numk];
  for(int i=0; i<numk; i++){
    field[i]= new Wavefield(gridsize);
    field[i]->restart(restartf);
  }
}

void Eqnset::stepwaveeq(double* Phi, Qhistory* qhistory){
  for(int i=0; i<numk; i++){
    weqnobj->stepwaveeq(uRe,uIm,qhistory,field[i],scalfactarr[i]);
    field[i]->update(uRe,uIm);
  }
//
// the following lines sum the U fields to form the Phi field
//
  double* URe=field[0]->U_1Re;
  double* UIm=field[0]->U_1Im;
  double* prefactRe=scalfactarr[0]->factRe;
  double* prefactIm=scalfactarr[0]->factIm;
  iphi=0;
  icentre=startfirstrow;
  for(long i=0; i<sidelength; i++){
    for(long j=0; j<sidelength; j++){
      Phi[iphi] = prefactRe[icentre]*URe[iphi] + prefactIm[icentre]*UIm[iphi];
      icentre++,iphi++;
    }
    icentre+=2;
  }
  for(int i=1; i<numk; i++){
    double* URe=field[i]->U_1Re;
    double* UIm=field[i]->U_1Im;
    double* prefactRe=scalfactarr[i]->factRe;
    double* prefactIm=scalfactarr[i]->factIm;
    iphi=0;
    icentre=startfirstrow;
    for(long k=0; k<sidelength; k++){
      for(long j=0; j<sidelength; j++){
        Phi[iphi] += prefactRe[icentre]*URe[iphi] + prefactIm[icentre]*UIm[iphi];
        icentre++,iphi++;
      }
      icentre+=2;
    }
  }
}

