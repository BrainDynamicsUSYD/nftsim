#ifndef DENDRITERAMP_H
#define DENDRITERAMP_H

#include"dendrite.h"
//#include"de.h"
using std::vector;

class DendriteRamp : public Dendrite
{
  DendriteRamp(void); 				// default constructor
  DendriteRamp(DendriteRamp& ); 	// no copy constructor
 
protected:
  struct DendriteDE : public DE
  {
    vector<double> alpha_vec, beta_vec;
    double alpha2, beta2;
    double t1, t2, t3, t4;
    virtual void init( const double vinit);
    DendriteDE( int nodes, double deltat) : DE(nodes, deltat, 3) {}
    virtual ~DendriteDE(void) {}
    virtual void rhs( const vector<double>& y, vector<double>& dydt );
  };
  DendriteDE* de;
  RK4* rk4;
  
  double time;
  

  virtual void init( Configf& configf );
public: 
  DendriteRamp( int nodes, double deltat, int index,
      const Propag& prepropag, const Couple& precouple );
  virtual ~DendriteRamp(void);
  virtual void step(void);

  inline const vector<double>& V(void) const {return (*de)[1];}
  virtual void output( Output& output ) const;
};
  
#endif
