/** @file qresponse.h
  @brief Defines the QResponse class, for the soma response of neural populations.

  Each neural population is associated with a QResponse object, which produces
  the soma response.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_QRESPONSE_H
#define NEUROFIELD_SRC_QRESPONSE_H

// Forward declaration to break circular collaboration
class QResponse;

// Other neurofield headers
#include "array.h"      // Array;
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "dendrite.h"   // Dendrite;
#include "nf.h"         // NF;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <string> // std::string;
#include <vector> // std::vector;

class QResponse : public NF {
  QResponse(const QResponse& ); // No copy constructor allowed.
  QResponse();                  // No default constructor allowed.

 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  std::string mode;
  double theta = 0.0; ///< Mean firing threshold.
  double sigma = 0.0; ///< Standard deviation of the firing threshold.
  double Q_max = 0.0; ///< Maximum firing rate.
  double a = 0.0, b = 0.0, c = 0.0, d = 0.0;

  Array<Dendrite> dendrites; ///< array of dendrites
  std::vector<Array<Dendrite>::size_type> dendrite_index; ///< indices of dendrites
  std::vector<double> v; ///< soma potential for the population

 public:
  QResponse( size_type nodes, double deltat, size_type index );
  ~QResponse() override;
  void step() override;
  virtual void add2Dendrite( size_type index, const Propagator& prepropag,
                             const Coupling& precouple, Configf& configf );

  virtual void fire( std::vector<double>& Q ) const;
  inline const std::vector<double>& V() const;
  void output( Output& output ) const override;
  virtual void outputDendrite( size_type index, Output& output ) const;
};

const std::vector<double>& QResponse::V() const {
  return v;
}

#endif //NEUROFIELD_SRC_QRESPONSE_H
