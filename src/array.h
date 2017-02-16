/** @file array.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_ARRAY_H
#define NEUROFIELD_SRC_ARRAY_H

// C++ standard library headers
#include <vector> // std::vector;

//#include <omp.h>

template<class T>
class Array {
  Array(Array&);   // no copy constructor allowed

  std::vector<T*> m;
 public:
  typedef typename std::vector<T>::size_type size_type;
  virtual void step(void);
  virtual void pstep(void); // parallel for loop over elements::loop

  void add(T* t);
  void add(std::vector<T*> t);
  bool empty(void) const;
  inline T* operator[]( size_type index ) const;
  size_type size(void) const;

  Array<T>(void);
  virtual ~Array(void);
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
bool Array<T>::empty(void) const {
  return m.empty();
}

template<class T>
void Array<T>::step(void) {
  for( size_type i=0; i<m.size(); i++ ) {
    m[i]->step();
  }
}

template<class T>
void Array<T>::pstep(void) {
  // Note pstep() is needed as well as step() because output must use
  // step so that it is not parallelized
  //#pragma omp parallel for num_threads(5)
  for( size_type i=0; i<m.size(); i++ ) {
    m[i]->step();
  }
}

template<class T>
Array<T>::Array(void) = default;

template<class T>
Array<T>::~Array(void) {
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
typename Array<T>::size_type Array<T>::size(void) const {
  return m.size();
}

#endif //NEUROFIELD_SRC_ARRAY_H
