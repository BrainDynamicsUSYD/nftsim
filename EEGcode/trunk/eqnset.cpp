/***************************************************************************
                          eqnset.cpp  -  Propagator for wave equation with
                                         K extention
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "eqnset.h"
#include "prefact.h"
#include<math.h>

Eqnset::Eqnset(long nodes, double deltat){
  gridsize=static_cast<long>((sqrt(nodes)+2)*(sqrt(nodes)+2));
  if (sqrt( static_cast<double>(nodes)) != floor(sqrt( static_cast<double>(nodes)))){
    cerr << "Wave equation solver assumes square grid. Nodes per population must be a perfect square number" << endl;
    exit(EXIT_FAILURE);
  }
  rowlength=static_cast<long>(sqrt(gridsize));
  longsidelength=rowlength-2;
  shortsidelength=rowlength-2;
  startfirstrow=longsidelength+3;
  uRe = new double[nodes];
  uIm = new double[nodes];
  weqnobj = new Weqn(gridsize, deltat);
}

Eqnset::Eqnset(long nodes,double deltat,long longside){
  longsidelength=longside;
  if (nodes%longsidelength != 0){
    cerr << "To define a rectangular grid nodes: " << nodes <<endl;
    cerr << "divided by Longside: " << longside << endl;
    cerr << "must have no remainder" << endl;
    exit(EXIT_FAILURE); 
  }
  shortsidelength=nodes/longsidelength;
  if(longsidelength<2 || shortsidelength<2){
    cerr << "Error: The shortest dimension in Waveeqnrect" << endl;
    cerr << "must be 2 or greater" << endl;
    exit(EXIT_FAILURE);
  }
  gridsize=(longsidelength+2)*(shortsidelength+2);
  startfirstrow=longsidelength+3;
  uRe = new double[nodes];
  uIm = new double[nodes];
  weqnobj = new Weqn(gridsize,deltat,longsidelength,shortsidelength);
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
    if (fieldRe[i]) delete fieldRe[i];
    if (fieldIm[i]) delete fieldIm[i];
    if (scalfactarr[i]) delete scalfactarr[i];
  }
  if (fieldRe) delete [ ] fieldRe;
  if (fieldIm) delete [ ] fieldIm;
  if (scalfactarr) delete [ ] scalfactarr;
}

void Eqnset::init(Istrm& inputf, Qhistory* qhistory){
  inputf.validate("Deltax",58);
  inputf >> deltax;
  weqnobj->init(inputf,deltax,qhistory);
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
  fieldRe = new Field *[numk];
  fieldIm = new Field *[numk];
  scalfactarr = new Prefact *[numk];
  for(int i=0; i<numk; i++){
    fieldRe[i]= new Field(gridsize,longsidelength,shortsidelength,"URe");
    fieldRe[i]->init(inputf);
    fieldIm[i]= new Field(gridsize,longsidelength,shortsidelength,"UIm");
    fieldIm[i]->init(inputf);
    scalfactarr[i]= new Prefact(gridsize,longsidelength,shortsidelength);
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
  dumpf << "Num. K:";
  dumpf << numk << endl;
  double* kvect;
  for(int i=0; i<numk; i++){
    kvect=karray[i];
    dumpf << ":" << kvect[0] << "," << kvect[1] << " ";
  }
//
// Dump wavefields
//
  for(int i=0; i<numk; i++){
    fieldRe[i]->dump(dumpf);
    fieldIm[i]->dump(dumpf);
  }
}

void Eqnset::restart(Istrm& restartf){
  restartf.validate("Deltax",58);
  restartf >> deltax;
  weqnobj->restart(restartf,deltax);
  restartf.validate("geometric centre",58);
  restartf >> centrex;
  restartf.validate("",58);
  restartf >> centrey;
  restartf.validate("Num. K",58);
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
  fieldRe = new Field *[numk];
  fieldIm = new Field *[numk];
  for(int i=0; i<numk; i++){
    fieldRe[i]= new Field(gridsize,longsidelength,shortsidelength,"URe");
    fieldRe[i]->restart(restartf);
    fieldIm[i]= new Field(gridsize,longsidelength,shortsidelength,"UIm");
    fieldIm[i]->restart(restartf);
  }
}

void Eqnset::stepwaveeq(double* Phi, Qhistory* qhistory){
  for(int i=0; i<numk; i++){
    weqnobj->stepwaveeq(uRe,uIm,qhistory,fieldRe[i],fieldIm[i],scalfactarr[i]);
    fieldRe[i]->update(uRe);
    fieldIm[i]->update(uIm);
  }
//
// the following lines sum the U fields to form the Phi field
//
  double* URe=fieldRe[0]->U_1;
  double* UIm=fieldIm[0]->U_1;
  double* prefactRe=scalfactarr[0]->factRe;
  double* prefactIm=scalfactarr[0]->factIm;
  iphi=0;
  icentre=startfirstrow;
  for(long i=0; i<shortsidelength; i++){
    for(long j=0; j<longsidelength; j++){
      Phi[iphi] = prefactRe[icentre]*URe[iphi] + prefactIm[icentre]*UIm[iphi];
      icentre++,iphi++;
    }
    icentre+=2;
  }
  for(int i=1; i<numk; i++){
    double* URe=fieldRe[i]->U_1;
    double* UIm=fieldIm[i]->U_1;
    double* prefactRe=scalfactarr[i]->factRe;
    double* prefactIm=scalfactarr[i]->factIm;
    iphi=0;
    icentre=startfirstrow;
    for(long k=0; k<shortsidelength; k++){
      for(long j=0; j<longsidelength; j++){
        Phi[iphi] += prefactRe[icentre]*URe[iphi] + prefactIm[icentre]*UIm[iphi];
        icentre++,iphi++;
      }
      icentre+=2;
    }
  }
}
