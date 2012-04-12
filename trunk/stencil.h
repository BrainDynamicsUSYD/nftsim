#ifndef STENCIL_H
#define STENCIL_H

#include<cstdlib>
#include<string>
using std::string;
#include<vector>
using std::vector;

class Stencil
{
  Stencil();
  Stencil& operator=(const Stencil&);
  Stencil(const Stencil&);
protected:
  int nodes;
  int longside;
  int shortside;
  string boundary;
  double* m;

  mutable int ptr;
public:
  Stencil( int nodes, int longside, const string& boundary );
  virtual ~Stencil(void);

  const vector<double>& operator= ( const vector<double>& field );
  virtual void operator++ (int i) const; // increment Moore grid
  void set( int node ) const; // point to node
  int get(void) const; // get ptr

  // get Moore grid values
  double nw() const; double n() const; double ne() const;
  double  w() const; double c() const; double  e() const;
  double sw() const; double s() const; double se() const;
};

#endif
