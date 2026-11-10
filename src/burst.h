/** @file burst.h
  @brief Declares BurstResponse (`Firing: Bursting`), a firing response with a burst-modulated threshold.

  BurstResponse is a sigmoid firing response whose threshold is modulated by
  two slow variables, Xtilde and Htilde, which model the currents (with
  conductances gX and gH) behind thalamic bursting. gX and gH can be ramped
  over time (gX2, tx1..tx4 and gH2, th1..th4). The equations are given at
  BurstResponse::BurstResponseDE::rhs().

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_BURST_H
#define NFTSIM_SRC_BURST_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "de.h"         // DE; RK4;
#include "output.h"     // Output;
#include "firing_response.h"  // FiringResponse;

// C++ standard library headers
#include <vector> // std::vector;

class BurstResponse : public FiringResponse { //derived class; constructor initializer
 private:

  struct BurstResponseDE : public DE {
    double taux = 0.0, tauh = 0.0;
    std::vector<double> xinfinity;
    virtual void init(double xinit, double hinit);
    BurstResponseDE( size_type nodes, double deltat) : DE(nodes, deltat, 2) {}
    ~BurstResponseDE() override = default;
    void rhs( const std::vector<double>& y, std::vector<double>& dydt, size_type n ) override;
  };
  BurstResponseDE de; ///< State {Xtilde, Htilde}.
  RK4 rk4;            ///< Integrates de (declared after it, so constructed after it).

  std::vector<double> modtheta;
  std::vector<double> gX,gH,xtilde,htilde;
  double ia = 0.0, ib = 0.0;
  double ic = 0.0;
  double taux = 0.0;  // time constant
  double tauh = 0.0;  // time constant
  double ax = 0.0;    // constant
  double mu = 0.0;    // constant

  std::vector<double> thetatemp, qfiring, xinfinity;

  double gX1 = 0.0, gX2 = 0.0, gH1 = 0.0, gH2 = 0.0;
  double tx1 = 0.0, tx2 = 0.0, tx3 = 0.0, tx4 = 0.0, th1 = 0.0, th2 = 0.0, th3 = 0.0, th4 = 0.0;
  double time;

 protected:
  void init( Configf& configf ) override;
 public:
  BurstResponse(const BurstResponse& ) = delete; // No copy constructor allowed.
  BurstResponse() = delete;                      // No default constructor allowed.

  BurstResponse( size_type nodes, double deltat, size_type index );
  ~BurstResponse() override; //destructor mem fnctn

  void step() override;
  void fire( std::vector<double>& Q ) const override;
  void output( Output& output ) const override; //vector of Output ptrs filled by
};

#endif //NFTSIM_SRC_BURST_H

