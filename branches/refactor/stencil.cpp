#include<iostream>
#include<vector>
using std::vector;
#include"stencil.h"

Stencil::Stencil( int nodes, int longside )
    : nodes(nodes), longside(longside), m(nodes)
{
  set(0);
}

Stencil::~Stencil(void)
{
}

void Stencil::operator=( const vector<double>& field )
{
  m = field;
  nw = m[_nw]; n  = m[_n]; ne = m[_ne];
  w  = m[_w]; c  = m[_c]; e  = m[_e];
  sw = m[_sw]; s  = m[_s]; se = m[_se];
}

void Stencil::operator++ (int i) const
{
  _nw++; _n++; _ne++;
  _w++; _c++; _e++;
  _sw++; _s++; _se++;

  if( (get()%longside)==longside-1 ) {      // centre at edge
    _ne -= longside; _e  -= longside; _se -= longside;
  }
  else if( (get()%longside)==0 ) { // centre wraps around edge
    _nw += longside; _w  += longside; _sw += longside;
    _ne += longside; _e  += longside; _se += longside;
  }
  else if( (get()%longside)==1 ) { // whole Moore grid passes edge
    _nw -= longside; _w  -= longside; _sw -= longside;
  }

  if( _nw>=nodes ) _nw -= nodes;
  if( _n >=nodes ) _n  -= nodes;
  if( _ne>=nodes ) _ne -= nodes;
  if( _w >=nodes ) _w  -= nodes;
  if( _c >=nodes ) _c  -= nodes;
  if( _e >=nodes ) _e  -= nodes;
  if( _sw>=nodes ) _sw -= nodes;
  if( _s >=nodes ) _s  -= nodes;
  if( _se>=nodes ) _se -= nodes;

  nw = m[_nw]; n  = m[_n]; ne = m[_ne];
  w  = m[_w]; c  = m[_c]; e  = m[_e];
  sw = m[_sw]; s  = m[_s]; se = m[_se];
}

void Stencil::set( int node ) const
{
  _c = node;
  _nw = _c-1-longside;
  _n  = _c  -longside;
  _ne = _c+1-longside;
  _w  = _c-1;
  _e  = _c+1;
  _sw = _c-1+longside;
  _s  = _c  +longside;
  _se = _c+1+longside;

  if( (get()%longside) == 0 ) {               // left edge
    _nw += longside;
    _w  += longside;
    _sw += longside;
  }
  else if( (get()%longside) == longside-1 ) { // right edge
    _ne += longside;
    _e  += longside;
    _se += longside;
  }
  else if( node < longside ) {                // top edge
    _nw += nodes;
    _n  += nodes;
    _ne += nodes;
  }
  else if( node >= nodes-longside ) {         // bottom edge
    _sw -= nodes;
    _s  -= nodes;
    _se -= nodes;
  }

  if(_nw<0) _nw+=nodes;
  if(_n <0) _n +=nodes;
  if(_ne<0) _ne+=nodes;

  nw = m[_nw]; n  = m[_n]; ne = m[_ne];
  w  = m[_w]; c  = m[_c]; e  = m[_e];
  sw = m[_sw]; s  = m[_s]; se = m[_se];
}

int Stencil::get(void) const
{
  return _c;
}
