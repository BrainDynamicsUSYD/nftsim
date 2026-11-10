/** @file test_stencil.cpp
  @brief Stencil and StencilLegacy neighbour indexing on periodic (torus) grids.

  Each node of a field holds its own index, so a neighbour lookup can be
  checked against the expected node on the torus. The walk covers the grid
  twice to check that operator++ wraps from the last node back to node 0.
  Before the fix in commit "Fix Stencil wrap-around on rectangular grids" the
  wrap happened at the wrong place on non-square grids.
*/

#include "unit_test.h"

#include "stencil.h"
#include "stencil_legacy.h"

#include <sstream>
#include <string>
#include <vector>

namespace {

/// Index of node (x, y) on a longside x shortside torus.
int torus_index( int x, int y, int longside, int shortside ) {
  x = ((x % longside) + longside) % longside;
  y = ((y % shortside) + shortside) % shortside;
  return y*longside + x;
}

/// Walks a Stencil over the grid twice. Returns "" if every neighbour is
/// correct, otherwise a description of the first error.
std::string walk_stencil( int nodes, int longside ) {
  const int shortside = nodes/longside;
  std::vector<double> field(nodes);
  for( int i=0; i<nodes; i++ ) {
    field[i] = i;
  }
  Stencil stencil(nodes, longside, "Torus");
  stencil = field;
  std::ostringstream err;
  for( int pass=0; pass<2; pass++ ) {
    for( int node=0; node<nodes; node++ ) {
      if( stencil.get() != node ) {
        err << "pass " << pass << ": expected node " << node << ", stencil is at " << stencil.get();
        return err.str();
      }
      const int x = node % longside;
      const int y = node / longside;
      struct { Stencil::Neighbours dir; const char* name; int expected; } cases[] = {
        {Stencil::c, "c", node},
        {Stencil::n, "n", torus_index(x, y-1, longside, shortside)},
        {Stencil::s, "s", torus_index(x, y+1, longside, shortside)},
        {Stencil::w, "w", torus_index(x-1, y, longside, shortside)},
        {Stencil::e, "e", torus_index(x+1, y, longside, shortside)},
      };
      for( const auto& c : cases ) {
        if( stencil(c.dir) != c.expected ) {
          err << "pass " << pass << ", node " << node << ": neighbour " << c.name
              << " is " << stencil(c.dir) << ", expected " << c.expected;
          return err.str();
        }
      }
      stencil++;
    }
  }
  return "";
}

/// As walk_stencil, for the nine-point StencilLegacy.
std::string walk_stencil_legacy( int nodes, int longside ) {
  const int shortside = nodes/longside;
  std::vector<double> field(nodes);
  for( int i=0; i<nodes; i++ ) {
    field[i] = i;
  }
  StencilLegacy stencil(nodes, longside, "Torus");
  stencil = field;
  std::ostringstream err;
  for( int pass=0; pass<2; pass++ ) {
    for( int node=0; node<nodes; node++ ) {
      if( stencil.get() != node ) {
        err << "pass " << pass << ": expected node " << node << ", stencil is at " << stencil.get();
        return err.str();
      }
      const int x = node % longside;
      const int y = node / longside;
      struct { StencilLegacy::Moore dir; const char* name; int dx, dy; } cases[] = {
        {StencilLegacy::nw, "nw", -1, -1}, {StencilLegacy::n, "n", 0, -1}, {StencilLegacy::ne, "ne", 1, -1},
        {StencilLegacy::w,  "w",  -1,  0}, {StencilLegacy::c, "c", 0,  0}, {StencilLegacy::e,  "e",  1,  0},
        {StencilLegacy::sw, "sw", -1,  1}, {StencilLegacy::s, "s", 0,  1}, {StencilLegacy::se, "se", 1,  1},
      };
      for( const auto& c : cases ) {
        const int expected = torus_index(x+c.dx, y+c.dy, longside, shortside);
        if( stencil(c.dir) != expected ) {
          err << "pass " << pass << ", node " << node << ": neighbour " << c.name
              << " is " << stencil(c.dir) << ", expected " << expected;
          return err.str();
        }
      }
      stencil++;
    }
  }
  return "";
}

} // namespace

TEST(stencil_square_torus) {
  std::string err = walk_stencil(64, 8);
  CHECK_MSG(err.empty(), err);
  err = walk_stencil(9, 3);
  CHECK_MSG(err.empty(), err);
}

TEST(stencil_rectangular_torus_wraps) {
  // 16 x 8 (the grid of test/numerical/wave-rectangular-grid.conf), and a
  // grid whose "longside" is the shorter side.
  std::string err = walk_stencil(128, 16);
  CHECK_MSG(err.empty(), "16x8: " << err);
  err = walk_stencil(32, 4);
  CHECK_MSG(err.empty(), "4x8: " << err);
  err = walk_stencil(10, 5);
  CHECK_MSG(err.empty(), "5x2: " << err);
}

TEST(stencil_legacy_square_torus) {
  std::string err = walk_stencil_legacy(64, 8);
  CHECK_MSG(err.empty(), err);
}

TEST(stencil_legacy_rectangular_torus_wraps) {
  std::string err = walk_stencil_legacy(128, 16);
  CHECK_MSG(err.empty(), "16x8: " << err);
  err = walk_stencil_legacy(32, 4);
  CHECK_MSG(err.empty(), "4x8: " << err);
}
