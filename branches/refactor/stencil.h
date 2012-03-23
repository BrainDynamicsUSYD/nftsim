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
  vector<double>* m;

  // pointers to the Moore grid
  int _nw; int _n; int _ne;
  int  _w; int _c; int  _e;
  int _sw; int _s; int _se;

  inline void update(void);
public:
  Stencil( int nodes, int longside );
  virtual ~Stencil(void);

  void assign( vector<double>* field );

  virtual void operator++(int i); // increment Moore grid
  void set( int node ); // point to node
  int get(void) const; // get current node index

  // get Moore grid
  double nw; double n; double ne;
  double  w; double c; double  e;
  double sw; double s; double se;
};

#endif
