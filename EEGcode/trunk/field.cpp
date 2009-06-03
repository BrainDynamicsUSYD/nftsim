/***************************************************************************
                          field.cpp  -   Store a field of data for 
                              propagator use with rectangular boundaries
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "field.h"
#include<math.h>

Field::Field(long gsize,long longside,long shortside,const char * fname):
               ident(fname),gridsize(gsize),longsidelength(longside),
	       shortsidelength(shortside){
  U_A = new double[gridsize];
  U_B = new double[gridsize];
  startfirstrow=longsidelength+3;
}


Field::~Field(){
  delete[ ] U_A;
  delete[ ] U_B;
}

void Field::init(Istrm& inputf){
  inputf.ignore(200,58); // Spike up to colon to find Initial value
  double Uinit;
  inputf >> Uinit;
  init(Uinit);
}

void Field::init(double Uinit){
  for(long i=0; i<gridsize; i++) U_A[i]=Uinit;
  for(long i=0; i<gridsize; i++) U_B[i]=Uinit;
  U_1=U_A;
  U_2=U_B;
}

void Field::init(double* Uinit){ // Alternate initializer
  copyarr(Uinit,U_A);
  for(long i=0; i<gridsize; i++){
    U_B[i]=U_A[i];
  }
  U_1=U_A;
  U_2=U_B;
}

void Field::dump(ofstream& dumpf){
  dumpf << ident << "_1: " ;
  for(long i=0; i<gridsize; i++)
    dumpf << U_1[i] << " ";
  dumpf << endl;
  dumpf << ident << "_2: " ;
  for(long i=0; i<gridsize; i++)
    dumpf << U_2[i] << " ";
  dumpf << endl;
}

void Field::restart(Istrm& restartf){
  U_1=U_A;
  U_2=U_B;
  restartf.ignore(200,58); //throw away endl and then ident _1: 
  for(long i=0; i<gridsize; i++){
    restartf >> U_1[i];
  }
  restartf.ignore(200,58); //throw away endl and then ident _2:
  for(long i=0; i<gridsize; i++){
    restartf >> U_2[i];
  }
  restartf.ignore(200,32); // throw away endl
}

void Field::update(double* U){
//
// Switch Point data formerly U_1[i] back to U_2[i] and vice versa
//
  double *tempU=U_2;
  U_2=U_1; // Switch pointers for U_2 to now point to data previously stored in U_1
  U_1=tempU; // Switch pointers for U_1 to now point to data previously stored in U_2
  copyarr(U, U_1); // Now update U_1 array with recent data
}

void Field::copyarr(double* Ufrom, double* Uto){
//
// Copy U[i] to U_1[i] taking into account array size differences
//
  double * pU=Ufrom; // Get pointer to start of U array the smaller array
  double * pU_1=Uto; // Get pointer to start of U_1 array the larger array
  // Next part copies middle of U values grid across
  double * __restrict__ pp1=pU_1+startfirstrow;
  double * __restrict__ pp=pU;
  for(long i=0; i<shortsidelength;i++){
    for(long j=0; j<longsidelength;j++){
       *pp1++=*pp++;
    }
    pp1+=2; // reposition at start of next row
  }
// Next part copies to top line based on last row
  pp1=pU_1+1;
  pp=pU+longsidelength*(shortsidelength-1);
  for(long i=0; i<longsidelength;i++)
    *pp1++=*pp++;
// Next part copies to bottom line based on first row
  pp1=pU_1+(longsidelength+2)*(shortsidelength+1)+1;
  pp=pU;
  for(long i=0; i<longsidelength;i++)
    *pp1++=*pp++;
// Next part copies to left line based on last column 
  pp1=pU_1+startfirstrow-1;
  pp=pU+longsidelength-1;
  for(long i=0; i<shortsidelength;i++){
    *pp1=*pp;
    pp1+=longsidelength+2;
    pp+=longsidelength;
  }
// Next part copies to right line based on first column 
  pp1=pU_1+startfirstrow+longsidelength;
  pp=pU;
  for(long i=0; i<shortsidelength;i++){
    *pp1=*pp;
    pp1+=longsidelength+2;
    pp+=longsidelength;
  }
// Next part copies four corners
  *pU_1=*(pU_1+longsidelength); // top left corner
  *(pU_1+longsidelength+1)=*(pU_1+1); // top right corner
  *(pU_1+gridsize-1)=*(pU_1+longsidelength+3); //bottom right corner
  *(pU_1+gridsize-longsidelength-2)=*(pU_1+gridsize-2); //bottom left corner
}
