/** @file burst.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_BURST_H
#define NEUROFIELD_SRC_BURST_H

// Other neurofield headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "qresponse.h"  // QResponse;

// C++ standard library headers
#include <vector> // std::vector;

class BurstResponse : public QResponse { //derived class; constructor initializer
 private:
  BurstResponse(const BurstResponse& ); // No copy constructor allowed.
  BurstResponse();                      // No default constructor allowed.

  void rk4(void);
  void rkderivs(std::vector<double>& xtemp, std::vector<double>& htemp,
                std::vector<double>& xk, std::vector<double>& hk);
  std::vector<double> modtheta;
  std::vector<double> gX,gH,xtilde,htilde,xtemp,htemp;
  std::vector<std::vector<double> > xk,hk; // stores temporary values in RK4
  double Veff = 0.0;
  double Vk = 0.0;
  double Vx = 0.0;
  double ia = 0.0, ib = 0.0;
  double ic = 0.0;
  double taux = 0.0;  // time constant
  double tauh = 0.0;  // time constant
  double ax = 0.0;    // constant
  double mu = 0.0;    // constant
  double h = 0.0, h2 = 0.0, h6 = 0.0;
  double yt[2],k1[2],k2[2],k3[2],k4[2]; // temporary RK4 values

  std::vector<double> thetatemp, qfiring, xinfinity;

  double gX1 = 0.0, gX2 = 0.0, gH1 = 0.0, gH2 = 0.0;
  double tx1 = 0.0, tx2 = 0.0, tx3 = 0.0, tx4 = 0.0, th1 = 0.0, th2 = 0.0, th3 = 0.0, th4 = 0.0;
  double time;

 protected:
  void init( Configf& configf ) override;
 public:
  BurstResponse( size_type nodes, double deltat, size_type index );
  ~BurstResponse(void) override; //destructor mem fnctn

  void step(void) override;
  void fire( std::vector<double>& Q ) const override;
  void output( Output& output ) const override; //vector of Output ptrs filled by
};

#endif //NEUROFIELD_SRC_BURST_H

