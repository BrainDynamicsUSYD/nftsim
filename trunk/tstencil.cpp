#include"tstencil.h"

TStencil::TStencil( int nodes, int longside )
    : Stencil(nodes,longside,0)
{
  set(0);
}

TStencil::~TStencil(void)
{
}

void TStencil::operator++ (int i)
{
  _nw++; _n++; _ne++;
  _w++; _c++; _e++;
  _sw++; _s++; _se++;

  if( (_c%longside)==longside-1 ) {      // centre at edge
    _ne -= longside; _e  -= longside; _se -= longside;
  }
  else if( (_c%longside)==0 ) { // centre wraps around edge
    _nw += longside; _w  += longside; _sw += longside;
    _ne += longside; _e  += longside; _se += longside;
  }
  else if( (_c%longside)==1 ) { // whole Moore grid passes edge
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
}

void TStencil::set( int node )
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

  if( (_c%longside) == 0 ) {               // left edge
    _nw += longside;
    _w  += longside;
    _sw += longside;
  }
  else if( (_c%longside) == longside-1 ) { // right edge
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
}

double TStencil::nw(void) const { return (*m)[_nw]; }
double TStencil:: n(void) const { return (*m)[_n ]; }
double TStencil::ne(void) const { return (*m)[_ne]; }
double TStencil:: w(void) const { return (*m)[_w ]; }
double TStencil:: c(void) const { return (*m)[_c ]; }
double TStencil:: e(void) const { return (*m)[_e ]; }
double TStencil::sw(void) const { return (*m)[_sw]; }
double TStencil:: s(void) const { return (*m)[_s ]; }
double TStencil::se(void) const { return (*m)[_se]; }
