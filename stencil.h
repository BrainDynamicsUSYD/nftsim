#ifndef STENCIL_H
#define STENCIL_H

#include<vector>
using std::vector;

class Stencil
{
  Stencil(); // no default constructor
  Stencil& operator=(const Stencil&);
  Stencil(const Stencil&);

  double bath;
protected:
  int nodes;
  int longside;
  vector<double>* m;

  // pointers to the Moore grid
  int _nw; int _n; int _ne;
  int  _w; int _c; int  _e;
  int _sw; int _s; int _se;
public:
  Stencil( int nodes, int longside, double bath=0 );
  virtual ~Stencil(void);

  void assign( vector<double>* field );

  virtual void operator++(int i); // increment Moore grid
  virtual void set( int node ); // point to node
  int get(void) const; // get current node index

  // get Moore grid
  virtual double nw()const; virtual double n()const; virtual double ne()const;
  virtual double  w()const; virtual double c()const; virtual double  e()const;
  virtual double sw()const; virtual double s()const; virtual double se()const;
};

#endif
