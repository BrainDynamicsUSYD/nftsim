#ifndef BURST_H
#define BURST_H

#include"qresponse.h"
#include"output.h"
#include <math.h>
#include <vector>
using std::vector;

class BurstResponse : public QResponse //derived class; constructor initializer
{
private:
  BurstResponse(BurstResponse& ); // no copy constructor; no explicit var name
  BurstResponse(void);      // no copy constructor; void no return value

  void rk4(void);
    void rkderivs(vector<double>& xtemp, vector<double>& htemp,vector<double>& xk, vector<double>& hk);
  vector<double> modtheta;
  vector<double> xtilde,htilde,xtemp,htemp;
  vector<vector<double> > xk,hk; // stores temporary values in RK4
  double ia; // Ia current density
  double ib; // Ib current density
  double ic; // Ic current density
  double taux; // time constant
  double tauh; // time constant
  double ax; // constant 
  double mu; // constant
  double h,h2,h6;
  double yt[2],k1[2],k2[2],k3[2],k4[2]; // temporary RK4 values

  vector<double> thetatemp, qfiring, xinfinity;
	
protected:
  void init( Configf& inputf );
  //void restart( Restartf& restartf );
  //void dump( Dumpf& dumpf ) const; //const member fnctn does not change obj
public: 
  BurstResponse( int nodes, double deltat, int index );
  virtual ~BurstResponse(void); //destructor mem fnctn 
								//virtual= needs redef in derived classes
  void step(void);
  void fire( vector<double>& Q ) const;
  virtual void output( Output& output ) const; //vector of Output ptrs filled by
  virtual void outputDendrite( int index, Output& output) const;
};

#endif

