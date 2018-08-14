/** @file array.h
  @brief Defines a container class template for components of a NF model.

  The Array class is used as a container for Population, Propagator, Coupling,
  and Dendrite classes. It provides a `step()` member function that iteratively
  calls the step method for each element.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_ARRAY_H
#define NFTSIM_SRC_ARRAY_H

// C++ standard library headers
#include <vector> // std::vector;

template<class T>
class Array {
  Array(const Array&); ///< No copy constructor allowed.

  std::vector<T*> m; ///< The class' main data, contains pointers to NF objects.
 public:
  using size_type = typename std::vector<T>::size_type;
  virtual void step(); ///< iteratively calls `step()` member of Array's elements.
  virtual void pstep(); ///< OpenMP parallel version of `Array::step()` //NOTE: OpenMP pragma currently disabled.

  void add(T* t); ///< adds a pointer to a NF object to the Array.
  void add(std::vector<T*> t); ///< adds a vector of pointers to NF objects to the Array.
  bool empty() const; ///< accesses underlying vector's `empty()` member.
  inline T* operator[]( size_type index ) const; ///< accesses underlying vector's `operator[]`.
  size_type size() const; ///< accesses underlying vector's `size()` member.

  Array<T>();
  virtual ~Array();
};

template<class T>
void Array<T>::add( T* t ) {
  m.push_back(t);
}

template<class T>
void Array<T>::add( std::vector<T*> t ) {
  for( size_type i=0; i<t.size(); i++ ) {
    m.push_back( t[i] );
  }
}

template<class T>
bool Array<T>::empty() const {
  return m.empty();
}

template<class T>
void Array<T>::step() {
  for( size_type i=0; i<m.size(); i++ ) {
    m[i]->step();
  }
}

template<class T>
void Array<T>::pstep() {
  // Note pstep() is needed as well as step() because output must use
  // step so that it is not parallelized
  //#pragma omp parallel for num_threads(5)
  for( size_type i=0; i<m.size(); i++ ) {
    m[i]->step();
  }
}

template<class T>
Array<T>::Array() = default;

template<class T>
Array<T>::~Array() {
  for( size_type i=0; i<m.size(); i++ ) {
    if( m[i] ) {
      delete m[i];
    }
  }
}

template<class T>
T* Array<T>::operator[]( size_type index ) const {
  return m[index];
}

template<class T>
typename Array<T>::size_type Array<T>::size() const {
  return m.size();
}

#endif //NFTSIM_SRC_ARRAY_H
