/** @file stencil_grid.h
  @brief StencilGrid: a field on a 2-D grid with a one-node halo, shared by
         Stencil and StencilLegacy.

  The grid has `longside` nodes per row and nodes/longside rows. It is stored
  with a one-node border (halo) on every side, so it holds
  (longside+2)*(shortside+2) values, and assigning a field fills the border
  according to the boundary conditions (Torus or Sphere). A cursor (`ptr`)
  walks the interior nodes in order with operator++, wrapping back to node 0
  after the last one. Stencil (five-point, used by Wave) and StencilLegacy
  (nine-point, used by WaveLegacy) add their neighbourhoods on top.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_STENCIL_GRID_H
#define NFTSIM_SRC_STENCIL_GRID_H

// C++ standard library headers
#include <string> // std::string;
#include <vector> // std::vector;

class StencilGrid {
 protected:
  /// Boundary conditions, parsed once by the constructor (operator= runs every step).
  enum class Boundary { Torus, Sphere };

  int nodes;    ///< Number of nodes covering the spatial domain.
  int longside; ///< Length of the long-side of the spatial domain (in number of nodes).
  int shortside; ///< Length of the short-side of the spatial domain (in number of nodes).
  Boundary boundary; ///< Type of boundary conditions.
  std::vector<double> storage; ///< The padded grid, row by row.
  double* m; ///< == storage.data(), for indexing with (signed) offsets.

  mutable int ptr; ///< Index into m of the current (centre) node.

  StencilGrid( int nodes, int longside, const std::string& boundary );
  ~StencilGrid();
 public:
  StencilGrid() = delete;                   ///< No default constructor allowed.
  StencilGrid(const StencilGrid&) = delete; ///< No copy constructor allowed.
  StencilGrid& operator=(const StencilGrid&) = delete; ///< No copy assignment operator allowed.

  /// Copies `field` into the grid, fills the halo and resets the cursor to node 0.
  const std::vector<double>& operator= ( const std::vector<double>& field );
  inline void operator++ (int) const; ///< Moves the cursor to the next node.
  void set( int node ) const; ///< Moves the cursor to `node`.
  int get() const; ///< Node at the cursor.
};

void StencilGrid::operator++ (int) const {
  ptr++;
  if( ( ptr%(longside+2)==longside+1 ) ) {
    // Move stencil from far side to near side
    ptr += 2;
  }
  if( ptr == (shortside+1)*(longside+2)+1 ) {
    // Move stencil from the very end of the array back to the first node
    set(0);
  }
}

#endif //NFTSIM_SRC_STENCIL_GRID_H
