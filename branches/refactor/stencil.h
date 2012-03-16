/***************************************************************************
                          stencil.h  -  rectangular (toroidal) Moore stencil
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

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
  vector<double> m;

  // pointers to the Moore grid
  mutable int _nw; mutable int _n; mutable int _ne;
  mutable int _w; mutable int _c; mutable int _e;
  mutable int _sw; mutable int _s; mutable int _se;

public:
  Stencil( int nodes, int longside );
  virtual ~Stencil(void);

  const vector<double>& operator=( const vector<double>& field );

  virtual void operator++(int i) const; // increment Moore grid
  void set( int node ) const; // point to node
  int get(void) const; // get current node index

  // get Moore grid
  mutable double nw; mutable double n; mutable double ne;
  mutable double w; mutable double c; mutable double e;
  mutable double sw; mutable double s; mutable double se;
};

#endif
