/** @file array.h
  @brief Defines a container class template for components of a NF model.

  The Array class owns the Population, Propagator, Coupling and Dendrite
  objects of a model. It provides a `step()` member function that calls the
  step method of each element in turn.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_ARRAY_H
#define NFTSIM_SRC_ARRAY_H

// C++ standard library headers
#include <memory>  // std::unique_ptr;
#include <utility> // std::move;
#include <vector>  // std::vector;

template<class T>
class Array {
  std::vector<std::unique_ptr<T>> m; ///< The elements, owned by the Array.
 public:
  using size_type = typename std::vector<std::unique_ptr<T>>::size_type;

  Array() = default;
  Array(const Array&) = delete;            ///< No copy constructor allowed.
  Array& operator=(const Array&) = delete; ///< No copy assignment allowed.

  /// Calls `step()` on each element, in the order they were added.
  void step() {
    for( auto& t : m ) {
      t->step();
    }
  }
  /// Adds an element, taking ownership of it.
  void add( std::unique_ptr<T> t ) {
    m.push_back( std::move(t) );
  }
  bool empty() const {
    return m.empty();
  }
  /// Non-owning pointer to element `index`.
  T* operator[]( size_type index ) const {
    return m[index].get();
  }
  size_type size() const {
    return m.size();
  }
};

#endif //NFTSIM_SRC_ARRAY_H
