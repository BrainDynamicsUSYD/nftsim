#include"stencil.h"

Stencil::Stencil( int nodes, int longside, double bath )
    : bath(bath), nodes(nodes), longside(longside), m(0)
{
  set(0);
}

Stencil::~Stencil(void)
{
}

void Stencil::assign( vector<double>* field )
{
  m = field;
  set(0);
}

void Stencil::operator++ (int i)
{
  _nw++; _n++; _ne++;
  _w++; _c++; _e++;
  _sw++; _s++; _se++;

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

void Stencil::set( int node )
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

  if(_nw<0) _nw+=nodes;
  if(_n <0) _n +=nodes;
  if(_ne<0) _ne+=nodes;
  if(_nw>=nodes) _nw-=nodes;
  if(_n >=nodes) _n -=nodes;
  if(_ne>=nodes) _ne-=nodes;
}

int Stencil::get(void) const
{
  return _c;
}

double Stencil::nw(void) const
{
  if( (_c%longside) == 0 || (_c<longside) )
    return bath;
  return (*m)[_nw];
}

double  Stencil::n(void) const
{
  if( _c<longside )
    return bath;
  return (*m)[_n ];
}

double Stencil::ne(void) const
{
  if( (_c<longside) || (_c%longside) == longside-1 )
    return bath;
  return (*m)[_ne];
}

double  Stencil::w(void) const
{
  if( (_c%longside) == 0 )
    return bath;
  return (*m)[_w ];
}

double  Stencil::c(void) const
{
  return (*m)[_c ];
}

double  Stencil::e(void) const
{
  if( (_c%longside) == longside-1 )
    return bath;
  return (*m)[_e ];
}

double Stencil::sw(void) const
{
  if( (_c%longside) == 0 || (_c>=nodes-longside) )
    return bath;
  return (*m)[_sw];
}

double  Stencil::s(void) const
{
  if( (_c>=nodes-longside) )
    return bath;
  return (*m)[_s ];
}

double Stencil::se(void) const
{
  if( (_c%longside) == longside-1 || (_c>=nodes-longside) )
    return bath;
  return (*m)[_se];
}
