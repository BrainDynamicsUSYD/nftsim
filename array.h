#ifndef ARRAY_H
#define ARRAY_H

#include<omp.h>
#include<string>
using std::string;
#include<vector>
using std::vector;
#include<iostream>
using std::endl;

template<class T>
class Array
{
  Array(Array&);   // no copy constructor allowed

  vector<T*> m;
public:
  virtual void step(void);
  virtual void pstep(void); // parallel for loop over elements::loop

  void add(T* t);
  void add(vector<T*> t);
  bool empty(void) const;
  T* operator[]( int index ) const;
  unsigned int size(void) const;

  Array<T>(void);
  virtual ~Array(void);
};

template<class T>
void Array<T>::add( T* t )
{
  m.push_back(t);
}

template<class T>
void Array<T>::add( vector<T*> t )
{
  for( size_t i=0; i<t.size(); i++ )
    m.push_back( t[i] );
}

template<class T>
bool Array<T>::empty(void) const
{
  return m.empty();
}

template<class T>
void Array<T>::step(void)
{
  for( size_t i=0; i<m.size(); i++ )
    m[i]->step();
}

template<class T>
void Array<T>::pstep(void)
{
  //#pragma omp parallel for num_threads(5)
  for( size_t i=0; i<m.size(); i++ )
    m[i]->step();
}

template<class T>
Array<T>::Array(void)
{
}

template<class T>
Array<T>::~Array(void)
{
  for( size_t i=0; i<m.size(); i++ )
    if( m[i] )
      delete m[i];
}

template<class T>
T* Array<T>::operator[]( int index ) const
{
  return m[index];
}

template<class T>
unsigned int Array<T>::size(void) const
{
  return m.size();
}

#endif
