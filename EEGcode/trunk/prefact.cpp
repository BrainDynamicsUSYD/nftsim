/***************************************************************************
                          prefact.cpp  - precalculated factor
                                         exp(-I* K dot r) 
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "prefact.h"
#include <math.h>

Prefact::Prefact(long gsize):gridsize(gsize){
  factRe= new double [gridsize];
  factIm= new double [gridsize];
}

Prefact::~Prefact(){
  delete [ ] factRe;
  delete [ ] factIm;
}

void Prefact::precalcfact(double* kvect, double deltax, long centrex, long centrey){
  long rowlength=static_cast<long>(sqrt(gridsize));
  long sidelength=rowlength-2;
  long longsidelength=rowlength-2;
  long shortsidelength=rowlength-2;
  long startfirstrow=longsidelength+3;
  long icentre=startfirstrow;
  double phaseang;
  for(long x=0; x<shortsidelength; x++){
    for(long y=0; y<longsidelength; y++){
      phaseang=deltax*(kvect[0]*(x-centrex) + kvect[1]*(y-centrey));
      factRe[icentre]=cos(phaseang);
      factIm[icentre]=-1.0*sin(phaseang);
      icentre++; //increment to next position
    }	      
    icentre+=2; //increment position to next row
  }
  wrapbndry(factRe,sidelength,rowlength);
  wrapbndry(factIm,sidelength,rowlength);
}

void Prefact::wrapbndry(double* array,long sidelength, long rowlength){
  double* pp;
  double* pp1;
// This part copies to top line based on last row
  pp1=array+1;
  pp=array+ (rowlength-1)*(rowlength-1);
  for(long i=0; i<sidelength;i++)
    *pp1++=*pp++;
// Next part copies to bottom line based on first row
  pp1=array+ rowlength*rowlength-rowlength+1;
  pp=array+ rowlength+1;
  for(long i=0; i<sidelength;i++)
    *pp1++=*pp++;
// Next part copies to left line based on last column
  pp1=array+rowlength;
  pp=array+ 2*rowlength-2;
  for(long i=0; i<sidelength;i++){
    *pp1=*pp;
    pp1+=rowlength;
    pp+=rowlength;
  }
// Next part copies to right line based on first column
  pp1=array+ 2*rowlength-1;
  pp=array+ rowlength+1;
  for(long i=0; i<sidelength;i++){
    *pp1=*pp;
    pp1+=rowlength;
    pp+=rowlength;
  }
}
