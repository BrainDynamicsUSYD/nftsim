/** @file population.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_POPULATION_H
#define NEUROFIELD_SRC_POPULATION_H

class Propagator;
class Coupling;
class Population;
class QResponse;

#include "timeseries.h"
#include "qresponse.h"
#include "dendrite.h"
#include "coupling.h"
#include "tau.h" // Must be included before propagator.h
#include "propagator.h"


using std::vector;


class Population : public NF {
  Population(void); // no default constructor
  Population(Population& ); // no copy constructor
  QResponse* qresponse; ///< qresponse for neural population
  Timeseries* timeseries; ///< timeseries for stimulus
 protected:
  vector< vector<double> >::size_type qkey; ///< index to the present q in qhistory
  vector< vector<double> > qhistory; ///< keyring of Q
  vector<double> q; ///< current Q, only for output purpose
  bool settled; ///< if true, forbids add2Dendrite and growHistory
  double length; ///< spatial length
  double qinit; ///< initial firing rate

  void init( Configf& configf ) override;
 public:
  Population( size_type nodes, double deltat, size_type index );
  ~Population() override;
  void step(void) override;
  virtual const vector<double>& Q( const Tau& tau ) const;
  double Qinit( Configf& configf ) const;
  virtual const vector<double>& V(void) const;
  inline double operator[]( size_type node ) const;
  const vector<double>& glu(void) const;
  inline double sheetlength(void) const {
    return length;
  }
  virtual void add2Dendrite( size_type index,
                             const Propagator& prepropag, const Coupling& precouple, Configf& configf );
  virtual void growHistory( const Tau& tau );
  void output( Output& output ) const override;
  virtual void outputDendrite( size_type
   index, Output& output ) const;
};

#endif //NEUROFIELD_SRC_POPULATION_H
