/***************************************************************************
                          random.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "random.h"
#include <math.h>
#include <iostream>
using std::cerr;

Random::Random(long seed):EPS(1.2e-7),RNMX(1.0-EPS),AM(1.0/IM){
  idum=seed;
  int j;
  long k;
  iv = new long[NTAB];
  if(-idum<1) idum=1;
  else idum=-idum;
  for(j=NTAB+7;j>=0;j--){
    k=idum/IQ;
    idum=IA*(idum-k*IQ)-IR*k;
    if(idum<0) idum+=IM;
    if(j<NTAB) iv[j]=idum;
  }
  iy=iv[0];
  ran();
}

Random::~Random(){
  delete [ ] iv;
}
 
double Random::ran(){
  int j;
  long k;
  double retval;

  k=idum/IQ;
  idum=IA*(idum-k*IQ)-IR*k;
  if(idum<0) idum+=IM;
  j=iy/NDIV;
  iy=iv[j];
  iv[j]=idum;
  if ((retval=static_cast<double>(AM*iy)) > RNMX) return RNMX;
  else return retval;
}

//
// Returns two gaussian random deviates
//  following a algorithm suggested by Knuth
//
void Random::gaussian(double& deviate1, double& deviate2){
  double x, y, norm, factor;
  do {
    x=2*ran() - 1; // throw two uniform deviates in square circumscribing unit circle
    y=2*ran() - 1;
    norm = x*x+y*y; 
  } while (norm > 1.0 || 0 == norm );  // test whether in unit circle
  factor=sqrt(-2.0*log(norm)/norm);
  deviate1=static_cast<double>(x*factor);
  deviate2=static_cast<double>(y*factor);
}
