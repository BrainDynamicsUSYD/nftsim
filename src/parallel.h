/** @file parallel.h
  @brief Optional OpenMP parallelism over nodes (prototype, `make OPENMP=1`).

  Without OpenMP the macros expand to nothing and nftsim is serial.
  Every parallel loop is over nodes and does the same arithmetic per node as
  the serial loop, with no reductions across nodes, so results are bit-for-bit
  the same for any number of threads.
*/

#ifndef NFTSIM_SRC_PARALLEL_H
#define NFTSIM_SRC_PARALLEL_H

/// Smallest number of nodes for which a loop is run in parallel.
#ifndef NFTSIM_OMP_MIN_NODES
#define NFTSIM_OMP_MIN_NODES 256
#endif

#ifdef _OPENMP
#define NFTSIM_PRAGMA(x) _Pragma(#x)
/// Parallel loop over nodes (the enclosing scope must have `nodes`).
#define NFTSIM_PARALLEL_FOR NFTSIM_PRAGMA(omp parallel for schedule(static) if(nodes >= NFTSIM_OMP_MIN_NODES))
#else
#define NFTSIM_PARALLEL_FOR
#endif

#endif //NFTSIM_SRC_PARALLEL_H
