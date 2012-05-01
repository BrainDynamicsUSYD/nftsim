#ifndef BURST_H
#define BURST_H

#include"qresponse.h"
#include"output.h"
#include <math.h>

class BurstResponse : public QResponse //derived class; constructor initializer
{
private:
  BurstResponse(BurstResponse& ); // no copy constructor; no explicit var name
  BurstResponse(void);      // no copy constructor; void no return value

  void rk4(void);
  void rkderivs(double* yt,double* dydt);
  
  double Ia; // Ia current density
  double Ib; // Ib current density
  double Ic; // Ic current density
  double taux; // time constant
  double tauh; // time constant
  double Ax; // constant 
  double Mu; // constant
  double initXtilde; // initial value for Xtilde
  double initHtilde; // initial value for Htilde
  double y[2]; // stores Htilde and Xtilde
  double yt[2],k1[2],k2[2],k3[2],k4[2]; // temporary RK4 values
  double h,h2,h6;

protected:
  void init( Configf& inputf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const; //const member fnctn does not change obj
public: 
  BurstResponse( int nodes, double deltat, int index );
  virtual ~BurstResponse(void); //destructor mem fnctn 
								//virtual= needs redef in derived classes
  void step(void);
  void fire( vector<double>& Q ) const; //const refers to object called on
  const vector<double>& V(void) const;
  vector<Output*> output(void) const; //vector of Output ptrs filled by
};

#endif

