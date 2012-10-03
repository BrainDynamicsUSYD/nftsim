#ifndef CMAP_H
#define CMAP_H

#include"wave.h"
#include"harmonic.h"

class CMap : public Wave, public Harmonic
{
  CMap(void);
  CMap(CMap&);
protected:
  void init( Configf& configf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;

  Tau edelay;
  vector< vector<double> > phistory; // keyring of phi
  int pkey; // index to present phi in phistory
public:
  CMap( int nodes, double deltat, int index, Population& prepop, Population& postpop, int longside, string topology );
  virtual ~CMap(void);
  virtual void step(void);
};

#endif
