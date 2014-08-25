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
  inline void operator++ (int) const; // increment Moore grid
  void set( int node ) const; // point to node
  int get(void) const; // get ptr

  inline double operator() ( Moore moore=c ) const {
    int arr[9] =   {ptr-longside-2-1,ptr-longside-2,ptr-longside-2+1,ptr-1,ptr,ptr+1,ptr+longside+2-1,ptr+longside+2,ptr+longside+2+1};
    return m[arr[moore+4]];
  }

  inline operator double (void) const {
    return (*this)(c);
  }
};


void Stencil::operator++ (int) const
{
  ptr++;
  if( ( ptr%(longside+2)==longside+1 ) ) // Move stencil from far side to near side
    ptr += 2;
  if( ptr == (longside+1)*(shortside+2)+1 ) // Move stencil from very end of the array to the
    set(0);
}

#endif
