#ifndef SINGLE_H
#define SINGLE_H

#include<cmath>
#include"qresponse.h"
#include"de.h"

class Single : public Population
{
  Single();
  Single(Single&);
  struct Singlede : public DE
  {
    double timescale;
    Singlede( int nodes, double deltat ) : DE(nodes,deltat,3) {}
    virtual ~Singlede(void) {}
    void rhs( const vector<double>& y, vector<double>& dydt );
  };
  Singlede de;
  RK4 rk4;
  vector<const Couple*> couples; // pointers to dendritic couplings
  vector<const Propag*> propags; // pointers to dendritic propagators
protected:
  void init( Configf& configf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dump ) const;
public:
  Single( int nodes, double deltat, int index );
  virtual ~Single();
  void step(void);
  void add2Dendrite( int index, const Propag& prepropag, const Couple& precouple, Configf& configf );
  double Qinit( Configf& configf ) const;
  const vector<double>& V(void) const;
  void output( Output& output ) const;
};

#endif
