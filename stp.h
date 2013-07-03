#ifndef STP_H
#define STP_H

#include"couple.h"
#include"de.h"

class STP : public virtual Couple
{
  STP();
  STP(STP&);
  struct STPde : public DE
  {
    double pos;
    double nu_0;
    double kappa;
    double phi_r;
    double t_Xi;
    double Xi_max;
    bool undersaturate;
    STPde( int nodes, double deltat ) : DE(nodes,deltat,5) {}
    virtual ~STPde(void) {}
    void rhs( const vector<double>& y, vector<double>& dydt );
  };
protected:
  STPde de;
  RK4 rk4;

  virtual void init( Configf& configf );
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;
public: 
  STP( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop );
  virtual ~STP(void);
  virtual void step(void);
  virtual const vector<double>& nu(void) const;
  virtual void output( Output& output ) const;
};

#endif
