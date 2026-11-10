/** @file stencil_legacy.h
  @brief StencilLegacy: the nine-point (Moore) neighbourhood used by WaveLegacy.

  A StencilLegacy is a StencilGrid (a field on a 2-D grid with a one-node
  halo, see stencil_grid.h) whose cursor gives the centre node and all eight
  neighbours of a square nine-point stencil:
  \f{matrix}{
    nw & n & ne \\
     w & c & e  \\
    sw & s & se
  \f}
  It is kept for WaveLegacy, which reproduces earlier published results.
  Stencil (stencil.h) is the five-point version used by Wave.

  https://en.wikipedia.org/wiki/Stencil_code

  @author Peter Drysdale, Felix Fung.
*/

#ifndef NFTSIM_SRC_STENCIL_LEGACY_H
#define NFTSIM_SRC_STENCIL_LEGACY_H

// Other nftsim headers
#include "stencil_grid.h" // StencilGrid;

// C++ standard library headers
#include <string> // std::string;

class StencilLegacy : public StencilGrid {
 public:
  enum Moore {
    nw=-4, n, ne,
    w,     c,  e,
    sw,    s, se
  };

  StencilLegacy( int nodes, int longside, const std::string& boundary )
    : StencilGrid(nodes, longside, boundary) {}

  using StencilGrid::operator=; // assign a field

  /// Value at a neighbour of the node at the cursor.
  inline double operator() ( Moore moore=c ) const {
    int arr[9] = {ptr-longside-2-1, ptr-longside-2, ptr-longside-2+1,
                  ptr-1,            ptr,            ptr+1,
                  ptr+longside+2-1, ptr+longside+2, ptr+longside+2+1};
    return m[arr[moore+4]];
  }

  inline operator double () const {
    return (*this)(c);
  }
};

#endif //NFTSIM_SRC_STENCIL_LEGACY_H
