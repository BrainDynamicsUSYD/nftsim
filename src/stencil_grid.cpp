/** @file stencil_grid.cpp
  @brief StencilGrid: a field on a 2-D grid with a one-node halo, shared by
         Stencil and StencilLegacy.

  operator= copies a field into the interior of the padded grid and fills the
  halo from the opposite edges (Torus; for Sphere the top and bottom halo rows
  are the opposite edge rows reversed), so the neighbours of every interior
  node can be read without wrapping indices.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "stencil_grid.h" // StencilGrid;

// Other nftsim headers
#include "parallel.h"     // NFTSIM_PARALLEL_FOR;
#include "restrict.h"     // NFTSIM_RESTRICT;

// C++ standard library headers
#include <cstdlib>  // std::exit; EXIT_FAILURE;
#include <iostream> // std::cerr; std::endl;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

StencilGrid::StencilGrid( int nodes, int longside, const string& boundary )
  : nodes(nodes), longside(longside), shortside(nodes/longside),
    boundary(Boundary::Torus), ptr(0) {
  if( boundary == "Sphere" ) {
    this->boundary = Boundary::Sphere;
  } else if( boundary != "Torus" ) {
    cerr<<"Stencil boundary must be Torus or Sphere, not '"<<boundary<<"'."<<endl;
    if( boundary.compare(0, 11, "Nonperiodic") == 0 ) {
      cerr<<"Nonperiodic boundaries are not implemented for Wave propagators."<<endl;
    }
    exit(EXIT_FAILURE);
  }
  storage.resize( static_cast<vector<double>::size_type>((shortside+2)*(longside+2)) );
  m = storage.data();
}

StencilGrid::~StencilGrid() = default;

const vector<double>& StencilGrid::operator= ( const vector<double>& field ) {
  // copy centre square, row by row
  {
    const double* NFTSIM_RESTRICT f = field.data();
    double* NFTSIM_RESTRICT g = m;
    const int L = longside;
    NFTSIM_PARALLEL_FOR
    for( int i=0; i<shortside; i++ ) {
      for( int j=0; j<L; j++ ) {
        g[(i+1)*(L+2)+j+1] = f[i*L+j];
      }
    }
  }
  if( boundary == Boundary::Torus ) {
    const double* NFTSIM_RESTRICT p1;
    double* NFTSIM_RESTRICT p2;

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


    // copy 4 corners
    m[0] = field[nodes-1];
    m[longside+1] = field[nodes-longside];
    m[(longside+2)*(nodes/longside+1)] = field[longside-1];
    m[(longside+2)*(nodes/longside+2)-1] = field[0];
  } else { // Boundary::Sphere
    const double* NFTSIM_RESTRICT p1;
    double* NFTSIM_RESTRICT p2;

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

    // copy 4 corners (the same as for a torus, which is not exact for a sphere)
    m[0] = field[nodes-1];
    m[longside+1] = field[nodes-longside];
    m[(longside+2)*(nodes/longside+1)] = field[longside-1];
    m[(longside+2)*(nodes/longside+2)-1] = field[0];
  }

  set(0);
  return field;
}

void StencilGrid::set( int node ) const {
  if( node>=0 && node<nodes ) {
    int x = node%longside;
    int y = node/longside;
    ptr = (y+1)*(longside+2) +x+1;
  } else {
    cerr<<"Stencil node setting out of bound: "<<node<<endl;
    exit(EXIT_FAILURE);
  }
}

int StencilGrid::get() const {
  int x = ptr%(longside+2)-1;
  int y = ptr/(longside+2)-1;
  return x+y*longside;
}

