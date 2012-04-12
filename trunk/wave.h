#ifndef WAVE_H
#define WAVE_H

#include"propag.h"
#include"stencil.h"

class Wave : public Propag
{
  Wave(); // no default constructor
  Wave(Wave&); // no copy constructor
protected:
  void init( Configf& configf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;

  Stencil* oldp[2]; // keyring stencil to past phi, oldp[key]==most recent
  Stencil* oldQ[2]; // keyring stencil to past Q, oldQ[key]==most recent
  int key;

  // variables that's initialized once only
  double deltax; // spatial length of a node
  double range;  // characteristic damping length
  double gamma;  // damping coefficient
  double dt2on12;
  double dfact;
  double dt2ondx2;
  double p2;
  double tenminus3p2;
  double twominus3p2;
  double exp1;
  double exp2;

  // variables that's changed every timestep
  double sump;
  double diagsump;
  double sumQ;
  double diagsumQ;
  double drive;
public: 
  Wave( int nodes, double deltat, int index, Population& prepop,
      Population& postpop, int longside, string topology );
  virtual ~Wave(void);
  virtual void step(void); 
};

#endif
