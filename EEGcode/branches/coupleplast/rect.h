// rect.h
// rectangular method for definite integrals

#ifndef RECT_H
#define RECT_H

#include "integrate.h"

class rect: public integrate
{
public:
  virtual double Int( double x0, double x1, double dx, double (*f)(double) );
  rect(void) {}
  virtual ~rect(void) {}
protected:
};

#endif
