#ifndef TSTENCIL_H
#define TSTENCIL_H

#include"stencil.h"

class TStencil : public Stencil
{
  TStencil(); // no default constructor
  TStencil& operator=(const TStencil&);
  TStencil(const TStencil&);
public:
  TStencil( int nodes, int longside );
  virtual ~TStencil(void);

  virtual void operator++(int i); // increment Moore grid
  virtual void set( int node ); // point to node

  // get Moore grid
  virtual double nw()const; virtual double n()const; virtual double ne()const;
  virtual double  w()const; virtual double c()const; virtual double  e()const;
  virtual double sw()const; virtual double s()const; virtual double se()const;
};

#endif
