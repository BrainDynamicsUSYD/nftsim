/** @file glutamate_response.h
  @brief Declares the GlutamateResponse class, for the soma response of neural populations.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_GLUTAMATE_RESPONSE_H
#define NFTSIM_SRC_GLUTAMATE_RESPONSE_H

// Forward declaration to break circular collaboration
class GlutamateResponse;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "de.h"         // DE; RK4;
#include "firing_response.h"  // FiringResponse;

// C++ standard library headers
#include <vector> // std::vector;

class GlutamateResponse : public FiringResponse {
 protected:
  void init( Configf& configf ) override;

  // glutamate concentration in synaptic cleft
  struct Glu : public DE {
    double Lambda = 0.0; ///< glutamate concentration per action potential
    double tGlu = 0.0;   ///< time scale of glutamate

    Glu( vvd_size_type nodes, double deltat ) : DE(nodes,deltat,2) {}
    ~Glu() override = default;
    void init( Configf& configf );
    void rhs( const std::vector<double>& y, std::vector<double>& dydt, size_type /*unused*/ ) override;
  };
  Glu glu_m;
  RK4 glu_rk4;

 public:
  GlutamateResponse(const GlutamateResponse& ) = delete; // No copy constructor allowed.
  GlutamateResponse() = delete;                          // No default constructor allowed.

  void step() override;
  GlutamateResponse( size_type nodes, double deltat, size_type index );
  ~GlutamateResponse() override;
  const std::vector<double>& glu() const;
};

#endif //NFTSIM_SRC_GLUTAMATE_RESPONSE_H
