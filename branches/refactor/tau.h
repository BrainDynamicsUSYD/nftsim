/***************************************************************************
                          tau.h  -  implements axonal time delay
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef TAU_H
#define TAU_H

#include<vector>
using std::vector;
#include"nf.h"

class Tau : public NF
{
  Tau(Tau& ); // no copy constructor
  int max; // if tau is nonhomogeneous, == biggest element
  vector<double> m; // tau values across nodes, size()==1 if homogeneous
protected:
  void init( Configf& configf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;
public: 
  Tau(void); // default is zero axonal delay
  Tau( int nodes, double deltat, int index );
  virtual ~Tau(void);
  void step(void);

  friend class Population;
};

#endif
