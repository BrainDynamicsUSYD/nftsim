/** @file stencil.h
  @brief Stencil declaration -- spatial neighbourhood used to evaluate \f[{\Delta}x\f].

  This file contains the Stencil declaration, including a definition of Stencil's
  grid increment operator (ie, ++), and the parentheses operator (ie, ()). The
  default neighbourhood is the orthogonal or von Neumann neighbourhood of a
  two-dimensional five-point stencil, that is:
  \f{matrix}{
      & n &   \\
    w & c & e \\
      & s
  \f}

  #TODO: a reference to a good review paper or textbook would be better here.
  https://en.wikipedia.org/wiki/Five-point_stencil
  https://en.wikipedia.org/wiki/Stencil_code

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_STENCIL_H
#define NFTSIM_SRC_STENCIL_H

// C++ standard library headers
#include <string> // std::string;
#include <vector> // std::vector;

class Stencil {
 protected:
  int nodes;    ///< Number of nodes covering the spatial domain.
  int longside; ///< Length of the long-side of the spatial domain (in number of nodes).
  int shortside; ///< Length of the short-side of the spatial domain (in number of nodes).
  std::string boundary; ///< Type of boundary conditions.
  double* m;

  mutable int ptr;
 public:
  Stencil() = delete;               ///< No default constructor allowed.
  Stencil(const Stencil&) = delete; ///< No copy constructor allowed.
  Stencil& operator=(const Stencil&) = delete; ///< No copy assignment operator allowed.

  enum Neighbours {
       n=-2,
    w, c, e,
       s
  };

  Stencil( int nodes, int longside, const std::string& boundary );
  virtual ~Stencil();

  const std::vector<double>& operator= ( const std::vector<double>& field );
  inline void operator++ (int) const; ///< increment Neighbours grid
  void set( int node ) const; ///< point to node
  int get() const; ///< get ptr

  inline double operator() ( Neighbours neighbour=c ) const {
    int arr[5] = {       ptr-longside-2,
                  ptr-1,       ptr,      ptr+1,
                         ptr+longside+2};
    return m[arr[neighbour+2]];
  }

  inline operator double () const {
    return (*this)(c);
  }
};

void Stencil::operator++ (int) const {
  ptr++;
  if( ( ptr%(longside+2)==longside+1 ) ) {
    // Move stencil from far side to near side
    ptr += 2;
  }
  if( ptr == (longside+1)*(shortside+2)+1 ) {
    // Move stencil from very end of the array to the
    set(0);
  }
}

#endif //NFTSIM_SRC_STENCIL_H
