/** @file parallel.h
  @brief Optional OpenMP parallelism over nodes (prototype, `make OPENMP=1`).

  With OpenMP, Solver::solve() runs the whole time loop inside one parallel
  region. Every thread calls every object's step(), and each object's per-node
  loops cover only the calling thread's block of nodes, par::range(nodes).
  The blocks are contiguous and fixed for the run, so in a chain of per-node
  loops (coupling, dendrites, soma sum, firing, propagator) each thread reads
  only values it wrote itself and no barrier is needed between them. The few
  steps that are not per node run on one thread: advancing a population's
  history index and white noise with par::single(), and output with
  par::serial(). Work that nothing reads until the next step (filling the
  Wave stencils' halos) is put off with par::atStepEnd() to the end of the
  step, so it needs no barriers of its own. A model with P populations
  (neural or stimulus) then has P+2 barriers per step, whatever the number
  of nodes, propagators and dendrites.

  Objects whose step() has not been written this way (nodeParallel() is false,
  see NF) are stepped by Array::step() inside par::serial(): one thread steps
  every node, as in the serial build.

  Without OpenMP, or outside a parallel region, par::range(n) is [0, n), the
  barriers do nothing and single() and serial() just call their argument, so
  the code runs as before. Every loop over nodes does the same arithmetic per
  node, in the same order, as the serial code, with no sums across nodes, so
  results are bit-for-bit the same for any number of threads.
*/

#ifndef NFTSIM_SRC_PARALLEL_H
#define NFTSIM_SRC_PARALLEL_H

// C++ standard library headers
#include <functional> // std::function;
#include <vector>     // std::vector;

#ifdef _OPENMP
#include <omp.h>
#endif

/// Smallest number of nodes for which Solver::solve() uses several threads.
#ifndef NFTSIM_OMP_MIN_NODES
#define NFTSIM_OMP_MIN_NODES 256
#endif

namespace nftsim {
namespace par {

using size_type = std::vector<double>::size_type;

/// The half-open range of nodes [begin, end).
struct Range {
  size_type begin;
  size_type end;
};

#ifdef _OPENMP
/// True while the calling thread runs the body of serial().
inline bool& in_serial() {
  static thread_local bool flag = false;
  return flag;
}
#endif

/// True if the calling thread is one of a team of threads sharing the nodes.
inline bool sharing() {
#ifdef _OPENMP
  return omp_in_parallel() && !in_serial();
#else
  return false;
#endif
}

/// The calling thread's block of the nodes [0, n): all of them unless sharing().
inline Range range( size_type n ) {
#ifdef _OPENMP
  if( sharing() ) {
    const auto nt = static_cast<size_type>(omp_get_num_threads());
    const auto t = static_cast<size_type>(omp_get_thread_num());
    const size_type q = n/nt;
    const size_type r = n%nt;
    const size_type begin = t*q + (t < r ? t : r);
    return Range{begin, begin + q + (t < r ? 1 : 0)};
  }
#endif
  return Range{0, n};
}

/// Waits until every thread of the team has reached this point.
inline void barrier() {
#ifdef _OPENMP
  if( sharing() ) {
#pragma omp barrier
  }
#endif
}

/// Runs f on one thread of the team; the others wait at the end until it is
/// done. Use it for state the following per-node loops read (eg an index into
/// a history), when no other thread can still be using that state.
template<class F> inline void single( F f ) {
#ifdef _OPENMP
  if( sharing() ) {
#pragma omp single
    {
      f();
    }
    return;
  }
#endif
  f();
}

/// Runs f on one thread once every thread has arrived; the others wait at the
/// end until it is done. f may read and write every node.
template<class F> inline void serial( F f ) {
#ifdef _OPENMP
  if( sharing() ) {
#pragma omp barrier
#pragma omp single
    {
      in_serial() = true;
      f();
      in_serial() = false;
    }
    return;
  }
#endif
  f();
}

/// Work put off to the end of the time-step by atStepEnd().
inline std::vector<std::function<void()>>& stepEndQueue() {
  static std::vector<std::function<void()>> queue;
  return queue;
}

/// Runs f now, or, while threads share the nodes, once on one thread at the
/// end of the time-step, when Solver::step() calls runStepEnd() (in a
/// serial() section, before output). Use it for work on state that nothing
/// reads again before the next step, which saves the barriers of a serial().
template<class F> inline void atStepEnd( F f ) {
#ifdef _OPENMP
  if( sharing() ) {
    if( omp_get_thread_num() == 0 ) { // every thread asks; one records it
      stepEndQueue().emplace_back(f);
    }
    return;
  }
#endif
  f();
}

/// Runs, in order, the work put off by atStepEnd(). Call on one thread only.
inline void runStepEnd() {
  for( auto& f : stepEndQueue() ) {
    f();
  }
  stepEndQueue().clear();
}

} // namespace par
} // namespace nftsim

#endif //NFTSIM_SRC_PARALLEL_H
