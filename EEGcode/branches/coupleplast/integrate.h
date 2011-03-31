// integrate.h
// provide the purely abstract class integrate for integration procedures

#ifndef INTEGRATE_H
#define INTEGRATE_H

// an abstract base class to enforce the interface for numerical integration
// the only interface is a static member function
// double Int( double x0, double x1, double dx, double (*f)(double) )

class integrate
{
public:
  virtual double Int( double x0, double x1, double dx, double (*f)(double) ) = 0; // purely abstract function
protected:
  integrate(void) {}
  virtual ~integrate(void) {}
};

#endif
