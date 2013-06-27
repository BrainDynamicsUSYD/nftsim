#ifndef STENCIL_H
#define STENCIL_H

#include<cstdlib>
#include<string>
using std::string;
#include<vector>
using std::vector;

enum Moore {
  nw=-4, n, ne,
   w,    c,  e,
  sw,    s, se
};

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

  inline double operator() ( Moore moore=c ) const {
    return m[ ptr +longside*(moore/3) +(moore%3) ];
  }
  inline operator double (void) const {
    return (*this)(c);
  }

  // get Moore grid values
  //double nw() const; double n() const; double ne() const;
  //double  w() const; double c() const; double  e() const;
  //double sw() const; double s() const; double se() const;
};

#endif
