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
  vector<double> gX,gH,xtilde,htilde,xtemp,htemp;
  vector<vector<double> > xk,hk; // stores temporary values in RK4
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

  vector<double> thetatemp, qfiring, xinfinity;

  double gX1, gX2, gH1, gH2;
  double tx1, tx2, tx3, tx4, th1, th2, th3, th4;
  double time; 
	
protected:
  void init( Configf& inputf );
public: 
  BurstResponse( int nodes, double deltat, int index );
  virtual ~BurstResponse(void); //destructor mem fnctn 

  void step(void);
  void fire( vector<double>& Q ) const;
  virtual void output( Output& output ) const; //vector of Output ptrs filled by
};

#endif

