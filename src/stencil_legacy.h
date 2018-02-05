/** @file stencil_legacy.h
  @brief StencilLegacy square nine-point stencil declaration.

  The StencilLegacy stencil declaration, including definition of the Moore
  neighbourhood and the stencil's Moore-grid increment operator (ie, ++).
  The specific Moore neighbourhood used here implements a "square" nine-point
  stencil.

  #TODO: a reference to a good review paper or textbook would be better here.
  https://en.wikipedia.org/wiki/Stencil_code

  @author Peter Drysdale, Felix Fung.
*/

#ifndef NFTSIM_SRC_STENCIL_LEGACY_H
#define NFTSIM_SRC_STENCIL_LEGACY_H

// C++ standard library headers
#include <string> // std::string;
#include <vector> // std::vector;

class StencilLegacy {
  StencilLegacy();
  StencilLegacy& operator=(const StencilLegacy&);
  StencilLegacy(const StencilLegacy&);
 protected:
  int nodes;
  int longside;
  int shortside;
  std::string boundary;
  double* m;

  mutable int ptr;
 public:
  enum Moore {
    nw=-4, n, ne,
    w,     c,  e,
    sw,    s, se
  };

  StencilLegacy( int nodes, int longside, const std::string& boundary );
  virtual ~StencilLegacy();

  const std::vector<double>& operator= ( const std::vector<double>& field );
  inline void operator++ (int) const; // increment Moore grid
  void set( int node ) const; // point to node
  int get() const; // get ptr

  inline double operator() ( Moore moore=c ) const {
    int arr[9] = {ptr-longside-2-1, ptr-longside-2, ptr-longside-2+1,
                  ptr-1,            ptr,            ptr+1,
                  ptr+longside+2-1, ptr+longside+2, ptr+longside+2+1};
    return m[arr[moore+4]];
  }

  inline operator double () const {
    return (*this)(c);
  }
};


void StencilLegacy::operator++ (int) const {
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

#endif //NFTSIM_SRC_STENCIL_LEGACY_H
