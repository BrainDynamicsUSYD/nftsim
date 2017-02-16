/** @file burst.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_BURST_H
#define NEUROFIELD_SRC_BURST_H

// C++ standard library headers
#include <vector> // std::vector;

// Neurofield headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "qresponse.h"  // QResponse;

class BurstResponse : public QResponse { //derived class; constructor initializer
 private:
  BurstResponse(BurstResponse& ); // no copy constructor; no explicit var name
  BurstResponse(void);      // no copy constructor; void no return value

  void rk4(void);
  void rkderivs(std::vector<double>& xtemp, std::vector<double>& htemp,
                std::vector<double>& xk, std::vector<double>& hk);
  std::vector<double> modtheta;
  std::vector<double> gX,gH,xtilde,htilde,xtemp,htemp;
  std::vector<std::vector<double> > xk,hk; // stores temporary values in RK4
  double Veff;
  double Vk;
  double Vx;
  double ia, ib;
  double ic;
  double taux;  // time constant
  double tauh;  // time constant
  double ax;    // constant
  double mu;    // constant
  double h,h2,h6;
  double yt[2],k1[2],k2[2],k3[2],k4[2]; // temporary RK4 values

  std::vector<double> thetatemp, qfiring, xinfinity;

  double gX1, gX2, gH1, gH2;
  double tx1, tx2, tx3, tx4, th1, th2, th3, th4;
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

