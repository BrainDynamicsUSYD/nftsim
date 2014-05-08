#ifndef COUPLE_H
#define COUPLE_H

#include"population.h"
#include"tau.h" // Must be included before propag.h

class Couple : public NF
{
  Couple();
  Couple(Couple&);
protected:
  virtual void init( Configf& configf ); 
  //virtual void restart( Restartf& restartf ); 
  //virtual void dump( Dumpf& dumpf ) const; 

  const Propag& prepropag;
  const Population& postpop;
  vector<double> n; // nu
  vector<double> P; // nu*phi
  int pos;
public: 
  Couple( int nodes, double deltat, int index,
      const Propag& prepropag, const Population& postpop );
  virtual ~Couple(void);
  virtual void step(void);
  double nuinit( Configf& configf ) const;
  virtual void output( Output& output ) const;
  virtual const vector<double>& nuphi(void) const;
  inline double operator[]( int node ) const;
  virtual bool excite(void) const;
};

double Couple::operator[]( int node ) const
{
  return P[node];
}

#endif
