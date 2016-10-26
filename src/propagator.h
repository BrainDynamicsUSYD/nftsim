/** @file propagator.h
  @brief The base Propagator class definition.

  Propagators are used to compute the axonal propagation of instantaneous
  firing rate.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_PROPAGATOR_H
#define NEUROFIELD_SRC_PROPAGATOR_H

#include"population.h"

using std::string;

class Propagator : public NF {
  Propagator(); // no default constructor
  Propagator(Propagator&); // no copy constructor
 protected:
  void init( Configf& configf ) override;
  //void restart( Restartf& restartf );
  //void dump( Dumpf& dumpf ) const;

  Population& prepop;
  Population& postpop;
  Tau tau; ///< tau_ab
  int longside;
  vector<double> p; ///< phi_ab
 public:
  Propagator( int nodes, double deltat, int index, Population& prepop,
          Population& postpop, int longside, string topology );
  ~Propagator(void) override;
  void step(void) override;
  double phiinit( Configf& configf ) const;
  virtual const vector<double>& phi(void) const;
  inline double operator[]( int node ) const;
  void output( Output& output ) const override;
};

double Propagator::operator[]( int node ) const {
  return p[node];
}

#endif
