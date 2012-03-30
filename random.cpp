#include <cmath>
#include "random.h"

Random::Random(int seed)
    :EPS(1.2e-7), RNMX(1.0-EPS), AM(1.0/IM)
{
  idum=seed;
  int j;
  int k;
  iv = new int[NTAB];
  if(-idum<1) idum=1;
  else idum=-idum;
  for( j=NTAB+7; j>=0; j--) {
    k = idum/IQ;
    idum = IA*( idum -k*IQ ) -IR*k;
    if( idum<0 ) idum += IM;
    if( j<NTAB ) iv[j] = idum;
  }
  iy = iv[0];
  ran();
}

Random::~Random(void)
{
  delete [ ] iv;
}
 
double Random::ran(void)
{
  int j;
  long k;
  double retval;

  k = idum/IQ;
  idum = IA*( idum -k*IQ ) -IR*k;
  if( idum<0 ) idum += IM;
  j = iy/NDIV;
  iy = iv[j];
  iv[j] = idum;
  retval=double(AM*iy);
  if ( retval>RNMX ) return RNMX;
  else return retval;
}

// Returns two gaussian random deviates following an algorithm suggested by Knuth
void Random::gaussian( double& deviate1, double& deviate2 )
{
  double x, y, norm, factor;
  do {
    // throw two uniform deviates in square circumscribing unit circle
    x =2*ran() -1;
    y =2*ran() -1;
    norm = x*x+y*y; 
  } while ( norm>1.0 || 0==norm );  // test whether in unit circle
  factor = sqrt( -2.0*log(norm)/norm );
  deviate1 = double(x*factor);
  deviate2 = double(y*factor);
}
