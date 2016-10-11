/** @file couple.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_COUPLE_H
#define NEUROFIELD_SRC_COUPLE_H

#include"population.h"

class Couple : public NF {
  Couple();
  Couple(Couple&);
 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  const Propag& prepropag;
  const Population& postpop;
  vector<double> n; ///< nu
  vector<double> P; ///< nu*phi
  int pos;
 public:
  Couple( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop );
  ~Couple(void) override;
  void step(void) override;
  double nuinit( Configf& configf ) const;
  void output( Output& output ) const override;
  virtual const vector<double>& nuphi(void) const;
  inline double operator[]( int node ) const;
  virtual bool excite(void) const;
};

double Couple::operator[]( int node ) const {
  return P[node];
}

#endif
