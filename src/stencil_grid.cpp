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
  assignNodes(field, 0, static_cast<vector<double>::size_type>(nodes));
  fillHalo();
  return field;
}

void StencilGrid::assignNodes( const vector<double>& field, vector<double>::size_type begin,
                               vector<double>::size_type end ) {
  const double* NFTSIM_RESTRICT p1 = field.data();
  double* NFTSIM_RESTRICT p2 = m;
  const auto L = static_cast<vector<double>::size_type>(longside);
  for( vector<double>::size_type i=begin; i<end; ) {
    // row by row: node i is at (i/L+1)*(L+2) + i%L + 1 in the padded grid
    const vector<double>::size_type row = i/L;
    const vector<double>::size_type rowend = (row+1)*L < end ? (row+1)*L : end;
    const vector<double>::size_type shift = (row+1)*(L+2) + 1 - row*L;
    for( ; i<rowend; i++ ) {
      p2[i+shift] = p1[i];
    }
  }
}

void StencilGrid::fillHalo() {
  // The halo is copied from the interior of the padded grid itself: interior
  // node (x, y) is at row_ (y+1), column (x+1).
  const int w = longside+2; // padded row length
  if( boundary == Boundary::Torus ) {
    // copy top row from bottom row
    for( int i=0; i<longside; i++ ) {
      m[1+i] = m[shortside*w +1+i];
    }
    // copy bottom row from top row
    for( int i=0; i<longside; i++ ) {
      m[(shortside+1)*w +1+i] = m[w +1+i];
    }
  } else { // Boundary::Sphere: the top and bottom halo rows are reversed
    // copy top row from bottom row, reversed. This writes m[longside+1]
    // (a corner, overwritten below) down to m[2], so m[1] is never set: kept
    // as it was (this looks like an off-by-one, but changing it would change
    // the output for Sphere).
    for( int i=0; i<longside; i++ ) {
      m[longside+1-i] = m[shortside*w +1+i];
    }
    // copy bottom row from top row, reversed
    for( int i=0; i<longside; i++ ) {
      m[(shortside+1)*w +1+i] = m[w +longside-i];
    }
  }
  // copy left edge from right edge, and right edge from left edge
  for( int i=1; i<=shortside; i++ ) {
    m[i*w] = m[i*w +longside];
    m[i*w +longside+1] = m[i*w +1];
  }
  // copy 4 corners (the same for a sphere as for a torus, which is not exact for a sphere)
  m[0] = m[shortside*w +longside];              // field[nodes-1]
  m[longside+1] = m[shortside*w +1];            // field[nodes-longside]
  m[(shortside+1)*w] = m[w +longside];          // field[longside-1]
  m[(shortside+2)*w -1] = m[w +1];              // field[0]
  set(0);
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

