/** @file make_unique.h
  @brief nftsim::make_unique, a C++11 stand-in for C++14's std::make_unique.

  nftsim::make_unique<T>(args...) returns a std::unique_ptr<T> owning
  `new T(args...)`. Use it to create objects whose owner holds them in a
  std::unique_ptr, eg `pops.add( nftsim::make_unique<Population>(...) )`.
  Always write it qualified, so it cannot clash with std::make_unique when
  compiling as C++14 or later.
*/

#ifndef NFTSIM_SRC_MAKE_UNIQUE_H
#define NFTSIM_SRC_MAKE_UNIQUE_H

// C++ standard library headers
#include <memory>  // std::unique_ptr;
#include <utility> // std::forward;

namespace nftsim {

template<class T, class... Args>
std::unique_ptr<T> make_unique( Args&&... args ) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace nftsim

#endif //NFTSIM_SRC_MAKE_UNIQUE_H
