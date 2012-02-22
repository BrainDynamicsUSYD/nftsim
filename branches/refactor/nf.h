/***************************************************************************
                          nf.h  -  abstract base class to specify interface
                                   of every other classes except configf
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef NF_H
#define NF_H

#include"configf.h"

class NF
{
  NF(void);      // no default constructor allowed
protected:
  virtual void init( Configf& configf ) = 0;
  virtual void restart( Restartf& restartf ) = 0;
  virtual void dump( Dumpf& dumpf ) const = 0;
  NF( int nodes, double deltat, int index );
  int nodes;  // number of nodes in simulation
  double deltat; // time increment per timestep
  int index; // object index within the population/connection model
public:
  friend Configf&  operator>> ( Configf& configf,   NF& nf );
  friend Restartf& operator>> ( Restartf& restartf, NF& nf );
  friend Dumpf&    operator<< ( Dumpf& dumpf, const NF& nf );
  virtual void step(void) = 0;

  virtual ~NF(void);
};

#endif
