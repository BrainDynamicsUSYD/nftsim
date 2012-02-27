/***************************************************************************
                          array.h  -  generic array class to hold objects,
                                      including populations, dendrites,
                                      propags, couples, and outputs
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include<string>
using std::string;
#include<vector>
using std::vector;
#include<iostream>
using std::endl;
#include"nf.h"

template<class T>
class Array : public NF
{
  Array(Array&);   // no copy constructor allowed
protected:
  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;

  vector<T*> m;
public:
  virtual void step(void);

  void add(T* t);
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
void Array<T>::init( Configf& configf )
{
  for( uint i=0; i<m.size(); i++ )
    configf>>*m[i];
}

template<class T>
void Array<T>::restart( Restartf& restartf )
{
  for( uint i=0; i<m.size(); i++ )
    restartf>>*m[i];
}

template<class T>
void Array<T>::dump( Dumpf& dumpf ) const
{
  for( uint i=0; i<m.size(); i++ )
    dumpf<<*m[i];
}

template<class T>
bool Array<T>::empty(void) const
{
  return m.empty();
}

template<class T>
void Array<T>::step(void)
{
  for( uint i=0; i<m.size(); i++ )
    m[i]->step();
}

template<class T>
Array<T>::Array(void)
    : NF(0,0,0)
{
}

template<class T>
Array<T>::~Array(void)
{
  for( uint i=0; i<m.size(); i++ )
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
