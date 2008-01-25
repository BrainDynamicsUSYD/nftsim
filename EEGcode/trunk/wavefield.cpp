/***************************************************************************
                          wavefield.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "wavefield.h"
#include<math.h>

Wavefield::Wavefield(long gsize):gridsize(gsize){
  U_ARe = new double[gridsize];
  U_AIm = new double[gridsize];
  U_BRe = new double[gridsize];
  U_BIm = new double[gridsize];
  rowlength=static_cast<long>(sqrt(gridsize));
  sidelength=rowlength-2;
  startfirstrow=rowlength+1;
  startlastrow=rowlength*sidelength+1;
}


Wavefield::~Wavefield(){
  delete[ ] U_ARe;
  delete[ ] U_AIm;
  delete[ ] U_BRe;
  delete[ ] U_BIm;
}

void Wavefield::init(Istrm& inputf){
  inputf.validate("Initial U",58);
  double U_initialRe;
  double U_initialIm;
  inputf >> U_initialRe;
  inputf.validate("",44); // find comma separating real and imaginary parts
  inputf >> U_initialIm;
  for(long i=0; i<gridsize; i++){
    U_ARe[i]=U_initialRe;
    U_AIm[i]=U_initialIm;
    U_BRe[i]=U_initialRe;
    U_BIm[i]=U_initialIm;
  }
  U_1Re=U_ARe;
  U_1Im=U_AIm;
  U_2Re=U_BRe;
  U_2Im=U_BIm;
}

void Wavefield::dump(ofstream& dumpf){
  dumpf << "U_1: " ;
  for(long i=0; i<gridsize; i++)
    dumpf << U_1Re[i] << "," << U_1Im[i] << " ";
  dumpf << endl;
  dumpf << "U_2: " ;
  for(long i=0; i<gridsize; i++)
    dumpf << U_2Re[i] << "," << U_2Im[i] << " ";
  dumpf << endl;
}

void Wavefield::restart(Istrm& restartf){
  U_1Re=U_ARe;
  U_1Im=U_AIm;
  U_2Re=U_BRe;
  U_2Im=U_BIm;
  restartf.ignore(200,58); //throw away endl and then U_1: 
  for(long i=0; i<gridsize; i++){
    restartf >> U_1Re[i];
    restartf.ignore(200,44);
    restartf >> U_1Im[i];
  }
  restartf.ignore(200,58); //throw away endl and the U_2:
  for(long i=0; i<gridsize; i++){
    restartf >> U_2Re[i];
    restartf.ignore(200,44);
    restartf >> U_2Im[i];
  }
  restartf.ignore(200,32); // throw away endl
}

void Wavefield::update(double* URe, double* UIm){
//
// Switch Point data formerly U_1[i] back to U_2[i] and vice versa
//
  double *tempURe=U_2Re;
  double *tempUIm=U_2Im;
  U_2Re=U_1Re; // Switch pointers for U_2 to now point to data previously stored in U_1
  U_2Im=U_1Im;
  U_1Re=tempURe; // Switch pointers for U_1 to now point to data previously stored in U_2
  U_1Im=tempUIm;
  copyarr(URe, U_1Re); // Now update U_1 array with recent data
  copyarr(UIm, U_1Im);
}

void Wavefield::copyarr(double* U, double* U_1){
//
// Copy U[i] to U_1[i] taking into account array size differences
//
  double * pU=U; // Get pointer to start of U array
  double * pU_1=U_1; // Get pointer to start of U_1 array
  // Next part copies middle of U values grid across
  double * pp1=pU_1+startfirstrow;
  double * pp=pU;
  for(long i=0; i<sidelength;i++){
    for(long j=0; j<sidelength;j++){
       *pp1++=*pp++;
    }
    pp1+=2; // reposition at start of next row
  }
// Next part copies top line based on last row
  pp1=pU_1+1;
  pp=pU+ sidelength*(sidelength-1);
  for(long i=0; i<sidelength;i++)
    *pp1++=*pp++;
// Next part copies bottom line based on first row
  pp1=pU_1+startlastrow+rowlength;
  pp=pU;
  for(long i=0; i<sidelength;i++)
    *pp1++=*pp++;
// Next part copies left line based on last column 
  pp1=pU_1+startfirstrow-1;
  pp=pU+sidelength-1;
  for(long i=0; i<sidelength;i++){
    *pp1=*pp;
    pp1+=rowlength;
    pp+=sidelength;
  }
// Next part copies right line based on first column 
  pp1=pU_1+startfirstrow+sidelength;
  pp=pU;
  for(long i=0; i<sidelength;i++){
    *pp1=*pp;
    pp1+=rowlength;
    pp+=sidelength;
  }
}
