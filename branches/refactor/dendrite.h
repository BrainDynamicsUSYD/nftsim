/***************************************************************************
                          dendrite.h  -  calculates dendritic response
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef DENDRITICR_H
#define DENDRITICR_H

#include<vector>
using std::vector;
#include"propag.h"
#include"couple.h"
#include"configf.h"
#include"nf.h"

class Propag;
class Couple;

class Dendrite : public NF
{
  Dendrite(void); // default constructor
  Dendrite(Dendrite& ); // no copy constructor

  vector<double> v;
  vector<double> dvdt;
  vector<double> oldnuphi;

  const Propag* prepropag;
  const Couple* precouple;

  double alpha;
  double beta;
protected:
  void init( Configf& configf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;
public: 
  Dendrite( int nodes, double deltat, int index,
      const Propag* prepropag, const Couple* precouple );
  virtual ~Dendrite(void);
  void step(void);
  const vector<double>& V(void) const;
};

#endif
