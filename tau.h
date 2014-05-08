#ifndef TAU_H
#define TAU_H

#include"nf.h"
using std::vector;
class Population;

class Tau : public NF
{
  Tau(Tau& ); // no copy constructor
  int max; // if tau is nonhomogeneous, == biggest element
  vector<int> m; // tau values across nodes, size()==1 if homogeneous
protected:
  void init( Configf& configf );
public: 
  Tau( int nodes, double deltat, int index );
  virtual ~Tau(void);
  void step(void);

  friend class Population;
};

#endif
