/***************************************************************************
                          propag.h  -  mapping propagator
                                       inherit for more sophisticated propagator
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PROPAG_H
#define PROPAG_H

#include"array.h"
#include"outputf.h"
#include"population.h"
#include"tau.h"
#include"configf.h"
#include"nf.h"

class Population;
class Tau;

class Propag : public NF
{
  Propag(); // no default constructor
  Propag(Propag&); // no copy constructor
protected:
  void init( Configf& configf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;

  const Population* const prepop;
  const Population* const postpop;
  Tau tau;
  int longside;
  vector<double> p;
public: 
  Propag( int nodes, double deltat, int index, const Population* const prepop,
      const Population* const postpop, int longside );
  virtual ~Propag(void);
  virtual void step(void); 
  virtual const vector<double>& phi(void) const;
  void output( Array<Outputf>& outputfs ) const;
};

#endif
