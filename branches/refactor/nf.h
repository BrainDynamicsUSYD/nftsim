/***************************************************************************
                          nf.h  -  abstract base class to specify interface
                                   of every other classes except configf
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef NF_H
#define NF_H

#include<fstream>

class Configf;
class Restartf;
typedef std::ofstream Dumpf;

class NF
{
protected:
  virtual void init( Configf& configf ) = 0;
  virtual void restart( Restartf& restartf ) = 0;
  virtual void dump( Dumpf& dumpf ) const = 0;
  NF(void);
  ~NF(void);
public:
  friend Configf&  operator>> ( Configf& configf,   NF& nf );
  friend Restartf& operator>> ( Restartf& restartf, NF& nf );
  friend Dumpf&    operator<< ( Dumpf& dumpf, const NF& nf );
  virtual void step(void) = 0;
};

#endif
