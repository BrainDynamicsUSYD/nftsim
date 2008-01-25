/***************************************************************************
                          random.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "random.h"

Random::Random():EPS(1.2e-7),RNMX(1.0-EPS),AM(1.0/IM){
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
 
float Random::ran(){
  int j;
  long k;
  float retval;

  k=idum/IQ;
  idum=IA*(idum-k*IQ)-IR*k;
  if(idum<0) idum+=IM;
  j=iy/NDIV;
  iy=iv[j];
  iv[j]=idum;
  if ((retval=static_cast<float>(AM*iy)) > RNMX) return RNMX;
  else return retval;
}
