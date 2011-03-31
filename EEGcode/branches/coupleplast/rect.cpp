#include "rect.h"

double rect::Int( double x0, double x1, double dx, double (*f)(double) )
{
  double result = 0;
  for( int i=0; i<=(x1-x0)/dx; i++ )
     result += f( x0+i*dx );
  return result*dx;
}
