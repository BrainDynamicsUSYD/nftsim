#include"ca2.h"

void Ca2::Ca2DE::rhs( const vector<double>& y, vector<double>& dydt )
{
  CaDE::rhs(y,dydt);
  double eta = y[4]+y[5]; double Omega = max*y[4]/(y[4]+y[5]);
  // nu
  dydt[3] =  y[6] -y[3]*eta;
  if( pos*(dydt[3]*deltat+y[3]) <0 ) dydt[3] = -y[3];
  // dnu/dt
  dydt[6] = eta*(Omega -y[3]);
}
