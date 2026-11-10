/** @file stencil.h
  @brief Stencil: the five-point (von Neumann) neighbourhood used by Wave.

  A Stencil is a StencilGrid (a field on a 2-D grid with a one-node halo, see
  stencil_grid.h) whose cursor gives the centre node and its four orthogonal
  neighbours:
  \f{matrix}{
      & n &   \\
    w & c & e \\
      & s
  \f}
  This is the five-point discrete Laplacian of Rennie (2001), Eq. A.16, used
  by Wave. StencilLegacy (stencil_legacy.h) is the nine-point version used by
  WaveLegacy.

  https://en.wikipedia.org/wiki/Five-point_stencil

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_STENCIL_H
#define NFTSIM_SRC_STENCIL_H

// Other nftsim headers
#include "stencil_grid.h" // StencilGrid;

// C++ standard library headers
#include <string> // std::string;

class Stencil : public StencilGrid {
 public:
  enum Neighbours {
       n=-2,
    w, c, e,
       s
  };

  Stencil( int nodes, int longside, const std::string& boundary )
    : StencilGrid(nodes, longside, boundary) {}

  using StencilGrid::operator=; // assign a field

  /// Value at a neighbour of the node at the cursor.
  inline double operator() ( Neighbours neighbour=c ) const {
    int arr[5] = {       ptr-longside-2,
                  ptr-1,       ptr,      ptr+1,
                         ptr+longside+2};
    return m[arr[neighbour+2]];
  }

  inline operator double () const {
    return (*this)(c);
  }
};

#endif //NFTSIM_SRC_STENCIL_H
