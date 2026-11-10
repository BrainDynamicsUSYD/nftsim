/** @file test_parallel.cpp
  @brief nftsim::par::range() blocks of nodes, and Stencil filled block by block.

  Without OpenMP, or outside a parallel region, range(n) is every node. In a
  parallel region (OpenMP build) the threads' blocks must be contiguous, in
  thread order, cover every node once and differ in size by at most one, also
  when there are fewer nodes than threads.
*/

#include "unit_test.h"

#include "parallel.h"
#include "stencil.h"

#include <vector>

using nftsim::par::Range;
using nftsim::par::range;
using nftsim::par::size_type;

TEST(parallel_range_serial) {
  const Range r = range(10);
  CHECK_EQ(r.begin, size_type(0));
  CHECK_EQ(r.end, size_type(10));
  CHECK(!nftsim::par::sharing());
}

#ifdef _OPENMP
TEST(parallel_range_blocks) {
  for( int threads : {1, 2, 3, 4, 7} ) {
    for( size_type n : {size_type(1), size_type(5), size_type(144), size_type(1025)} ) {
      std::vector<Range> blocks(static_cast<size_type>(threads));
      int team = 0;
#pragma omp parallel num_threads(threads)
      {
        blocks[static_cast<size_type>(omp_get_thread_num())] = range(n);
#pragma omp single
        team = omp_get_num_threads();
      }
      CHECK_EQ(team, threads); // the runtime gave us the team we asked for
      size_type next = 0;
      for( const Range& r : blocks ) {
        CHECK_EQ(r.begin, next);
        CHECK(r.end >= r.begin);
        CHECK(r.end - r.begin <= n/static_cast<size_type>(threads) + 1);
        next = r.end;
      }
      CHECK_EQ(next, n);
    }
  }
}
#endif

/// A Stencil filled by threads each copying their own block, then one filling
/// the halo, holds the same values as one assigned the whole field at once.
TEST(stencil_assign_by_blocks) {
  for( const char* boundary : {"Torus", "Sphere"} ) {
    const int longside = 6;
    const int nodes = longside*4;
    std::vector<double> field(static_cast<size_type>(nodes));
    for( int i=0; i<nodes; i++ ) {
      field[static_cast<size_type>(i)] = 0.5 + i;
    }
    Stencil whole(nodes, longside, boundary);
    whole = field;
    Stencil blocks(nodes, longside, boundary);
    blocks = std::vector<double>(static_cast<size_type>(nodes), 0.0); // as whole was before its assignment
#ifdef _OPENMP
#pragma omp parallel num_threads(3)
#endif
    {
      const Range r = range(static_cast<size_type>(nodes));
      blocks.assignNodes(field, r.begin, r.end);
      nftsim::par::barrier();
      nftsim::par::single( [&]{ blocks.fillHalo(); } );
    }
    for( int k=0; k<(nodes/longside+2)*(longside+2); k++ ) {
      if( k == 1 && boundary[0] == 'S' ) {
        continue; // never written for Sphere (see StencilGrid::fillHalo)
      }
      CHECK_MSG(whole.data()[k] == blocks.data()[k], boundary << " padded index " << k);
    }
  }
}
