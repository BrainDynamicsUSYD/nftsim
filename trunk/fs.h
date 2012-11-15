#ifndef FS_H
#define FS_H

#include"qresponse.h"
#include"de.h"

class FS : public QResponse
{
  FS(FS&);
  FS(void);
  struct FSde : public DE
  {
    double a_x;
    double tau_X;
    double tau_H;
    FSde( int nodes, double deltat ) : DE(nodes,deltat,3) {}
    virtual ~FSde(void) {}
    void rhs( const vector<double>& y, vector<double>& dydt );
  };
protected:
  FSde de;
  RK4 rk4;
  double mu; // conductance
  vector<double> I_theta;
  double A;
  double I_a;
  double I_b;
  double I_c;

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public:
  FS( int nodes, double deltat, int index );
  virtual ~FS(void);
  virtual void step(void);
  virtual void fire( vector<double>& Q ) const;
  virtual void output( Output& output ) const;
};

#endif
