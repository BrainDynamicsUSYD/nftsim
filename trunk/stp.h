#ifndef STP_H
#define STP_H

#include"couple.h"
#include"de.h"

class STP : public virtual Couple
{
  STP();
  STP(STP&);
  struct STPde : public RK4
  {
    double pos;
    double nu_0;
    double kappa;
    double phi_r;
    double t_Xi;
    double Xi_max;
    STPde( int nodes, double deltat ) : RK4(4,nodes,deltat) {}
    virtual ~STPde(void) {}
    void rhs( const vector<double>& y, vector<double>& dydt ) {
      // y == { phi, Xi, nu, oldphi }
      // phi, leave alone
      dydt[0] = 0;
      // Xi
      double dXidphi = Xi_max*kappa/phi_r*pow( y[0]/phi_r, kappa-1 )
                *pow(1+pow( y[0]/phi_r, kappa ),-2);
      dydt[1] = dXidphi*(y[0]-y[3])/deltat -y[0]/t_Xi;
      if( y[0]+dydt[0]*deltat<0 ) dydt[0] = -y[0];
      // nu
      dydt[2] = nu_0 *log(y[0]/y[3]) *( dXidphi -y[1]/y[0] );
      if( pos*(y[2]+dydt[2]*deltat)<0 ) dydt[2] = -y[2];
      // oldphi, leave alone
      dydt[3] = 0;
    }
  };
protected:
  double nu_0;
  double phi_r;
  double kappa;
  double t_Xi;
  //vector<double> oldphi;
  //vector<double> transmitter;
  STPde Xi_nu;

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  STP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop );
  virtual ~STP(void);
  virtual void step(void);
  virtual const vector<double>& nu(void) const;
  virtual vector<Output*> output(void) const;
};

#endif
