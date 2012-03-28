#ifndef STENCIL_H
#define STENCIL_H

#include<vector>
using std::vector;

class Stencil
{
  Stencil(); // no default constructor
  Stencil& operator=(const Stencil&);
  Stencil(const Stencil&);
protected:
  int nodes;
  int longside;
  vector<double>* m;

  // pointers to the Moore grid
  int _nw; int _n; int _ne;
  int  _w; int _c; int  _e;
  int _sw; int _s; int _se;

  //void update(void);
public:
  Stencil( int nodes, int longside );
  virtual ~Stencil(void);

  void assign( vector<double>* field );

  virtual void operator++(int i); // increment Moore grid
  virtual void set( int node ); // point to node
  int get(void) const; // get current node index

  // get Moore grid
  double nw(void) const; double n(void) const; double ne(void) const;
  double  w(void) const; double c(void) const; double  e(void) const;
  double sw(void) const; double s(void) const; double se(void) const;
};

#endif
