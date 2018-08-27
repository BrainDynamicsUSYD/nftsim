/** @file stencil_legacy.cpp
  @brief StencilLegacy, square nine-point stencil definition.

  As well as a constructor and destructor, this file defines the set() and
  get() member functions and a custom copy assignment operator(=).

  @author Peter Drysdale, Felix Fung.
*/

// Main module header
#include "stencil_legacy.h"  // StencilLegacy

// C++ standard library headers
#include <iostream> // std::cerr; std::endl;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

StencilLegacy::StencilLegacy( int nodes, int longside, const string& boundary )
  : nodes(nodes), longside(longside), shortside(nodes/longside),
    boundary(boundary), ptr(0) {
  if( boundary != "Torus" && boundary != "Sphere" ) {
    cerr<<"StencilLegacy boundary must be Torus or Sphere."<<endl;
    exit(EXIT_FAILURE);
  }
  m = new double[ (shortside+2)*(longside+2) ];
}

StencilLegacy::~StencilLegacy() {
  delete[] m;
}

const vector<double>& StencilLegacy::operator= ( const vector<double>& field ) {
  if( boundary == "Torus" ) {
    // copy centre square
    const double* __restrict__ p1 = &field[0];
    double* __restrict__ p2 = &m[3+longside];
    for( int i=0; i<shortside; i++ ) {
      for( int j=0; j<longside; j++ ) {
        *p2++ = *p1++;
      }
      p2 += 2;
    }

    // copy top row from bottom row
    p1 = &field[longside*(shortside-1)];
    p2 = &m[1];
    for( int i=0; i<longside; i++ ) {
      *p2++ = *p1++;
    }

    // copy bottom row from top row
    p1 = &field[0];
    p2 = &m[(longside+2)*(shortside+1) +1];
    for( int i=0; i<longside; i++ ) {
      *p2++ = *p1++;
    }

    // copy left edge from right edge
    p1 = &field[longside -1];
    p2 = &m[longside +2];
    for( int i=0; i<shortside; i++, p1+=longside, p2+=longside+2 ) {
      *p2 = *p1;
    }

    // copy right edge from left edge
    p1 = &field[0];
    p2 = &m[2*longside +3];
    for( int i=0; i<shortside; i++, p1+=longside, p2+=longside+2 ) {
      *p2 = *p1;
    }

    /*// copy centre square
    for( int j=0; j< nodes/longside; j++ )
      for( int i=0; i<longside; i++ )
        m[(j+1)*(longside+2)+i+1] = field[j*longside+i];

    // copy right edge into left boundary
    for( int i=0; i<=nodes/longside; i++ )
      m[i*(longside+2)] = field[i*longside-1];

    // copy left edge into right boundary
    for( int i=0; i<=nodes/longside; i++ )
      m[(i+2)*(longside+2)-1] = field[i*longside];

    // copy bottom edge into top boundary
    for( int i=0; i<longside; i++ )
      m[i+1] = field[nodes-longside+i];

    // copy top edge into bottom boundary
    for( int i=0; i<longside; i++ )
      m[(nodes/longside+1)*(longside+2)+1+i] = field[i];*/

    // copy 4 corners
    m[0] = field[nodes-1];
    m[longside+1] = field[nodes-longside];
    m[(longside+2)*(nodes/longside+1)] = field[longside-1];
    m[(longside+2)*(nodes/longside+2)-1] = field[0];
  } else if( boundary == "Sphere" ) {
    // copy centre square
    const double* __restrict__ p1 = &field[0];
    double* __restrict__ p2 = &m[3+longside];
    for( int i=0; i<shortside; i++ ) {
      for( int j=0; j<longside; j++ ) {
        *p2++ = *p1++;
      }
      p2 += 2;
    }

    // copy top row from bottom row
    p1 = &field[longside*(shortside-1)];
    p2 = &m[longside+1];
    for( int i=0; i<longside; i++ ) {
      *p2-- = *p1++;
    }

    // copy bottom row from top row
    p1 = &field[longside-1];
    p2 = &m[(longside+2)*(shortside+1) +1];
    for( int i=0; i<longside; i++ ) {
      *p2++ = *p1--;
    }

    // copy left edge from right edge
    p1 = &field[longside -1];
    p2 = &m[longside +2];
    for( int i=0; i<shortside; i++, p1+=longside, p2+=longside+2 ) {
      *p2 = *p1;
    }

    // copy right edge from left edge
    p1 = &field[0];
    p2 = &m[2*longside +3];
    for( int i=0; i<shortside; i++, p1+=longside, p2+=longside+2 ) {
      *p2 = *p1;
    }

    // copy 4 corners NEEDS TO MODIFY SOMEWHERE!!
    m[0] = field[nodes-1];
    m[longside+1] = field[nodes-longside];
    m[(longside+2)*(nodes/longside+1)] = field[longside-1];
    m[(longside+2)*(nodes/longside+2)-1] = field[0];
  }

  set(0);
  return field;
}

void StencilLegacy::set( int node ) const {
  if( node>=0 && node<nodes ) {
    int x = node%longside;
    int y = node/longside;
    ptr = (y+1)*(longside+2) +x+1;
  } else {
    cerr<<"StencilLegacy node setting out of bound: "<<node<<endl;
    exit(EXIT_FAILURE);
  }
}

int StencilLegacy::get() const {
  int x = ptr%(longside+2)-1;
  int y = ptr/(longside+2)-1;
  return x+y*longside;
}

/*double StencilLegacy::nw(void) const { return m[ ptr-longside-2-1]; }
double StencilLegacy::n (void) const { return m[ ptr-longside-2  ]; }
double StencilLegacy::ne(void) const { return m[ ptr-longside-2+1]; }
double StencilLegacy:: w(void) const { return m[ ptr           -1]; }
double StencilLegacy:: c(void) const { return m[ ptr             ]; }
double StencilLegacy:: e(void) const { return m[ ptr           +1]; }
double StencilLegacy::sw(void) const { return m[ ptr+longside+2-1]; }
double StencilLegacy::s (void) const { return m[ ptr+longside+2  ]; }
double StencilLegacy::se(void) const { return m[ ptr+longside+2+1]; }*/
