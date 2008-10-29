/***************************************************************************
                          prefact.cpp  - precalculated factor
                                         exp(-I* K dot r) 
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "prefact.h"
#include <math.h>

Prefact::Prefact(long gsize,long longside,long shortside):
    gridsize(gsize),longsidelength(longside),shortsidelength(shortside){
  factRe= new double [gridsize];
  factIm= new double [gridsize];
}

Prefact::~Prefact(){
  delete [ ] factRe;
  delete [ ] factIm;
}

void Prefact::precalcfact(double* kvect, double deltax, long centrex, long centrey){
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
  wrapbndry(factRe);
  wrapbndry(factIm);
}

void Prefact::wrapbndry(double* array){
  double* pp;
  double* pp1;
// This part copies to top line based on last row
  pp1=array+1;
  pp=array+ (longsidelength+2)*shortsidelength+1;
  for(long i=0; i<longsidelength;i++)
    *pp1++=*pp++;
// Next part copies to bottom line based on first row
  pp1=array+ (longsidelength+2)*(shortsidelength+1)+1;
  pp=array+ longsidelength+3;
  for(long i=0; i<longsidelength;i++)
    *pp1++=*pp++;
// Next part copies to left line based on last column
  pp1=array+longsidelength+2;
  pp=array+ 2*longsidelength+2;
  for(long i=0; i<shortsidelength;i++){
    *pp1=*pp;
    pp1+=longsidelength+2;
    pp+=longsidelength+2;
  }
// Next part copies to right line based on first column
  pp1=array+ 2*longsidelength+3;
  pp=array+ longsidelength+3;
  for(long i=0; i<shortsidelength;i++){
    *pp1=*pp;
    pp1+=longsidelength+2;
    pp+=longsidelength+2;
  }
// Next part copies four corners
  *array=*(array+longsidelength); // top left corner
  *(array+longsidelength+1)=*(array+1); // top right corner
  *(array+gridsize-1)=*(array+longsidelength+3); //bottom right corner
  *(array+gridsize-longsidelength-2)=*(array+gridsize-2); //bottom left corner
}
