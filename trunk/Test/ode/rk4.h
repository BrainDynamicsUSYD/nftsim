#ifndef RUNGE_KUTTA
#define RUNGE_KUTTA

#include"de.h"

class RK4
{
	RK4(void);
protected:
	double deltat;
public:
	RK4( double deltat ) : deltat(deltat) {}
	void operator() ( vector<double>& y, vector<double>& dydt, const DE& de ) const
	{
		int n = y.size();
		vector<double> dym(n);
		vector<double> dyt(n);
		vector<double> yt(n);
		double hh = deltat/2.;
		double h6 = deltat/6.;

		for( int i=0; i<n; i++ )
			yt[i] = y[i] +hh*dydt[i];
		de(yt,dyt);
		for( int i=0; i<n; i++ )
			yt[i] = y[i] +hh*dyt[i];
		de(yt,dym);
		for( int i=0; i<n; i++ ) {
			yt[i] = y[i] +deltat*dym[i];
			dym[i] += dyt[i];
		}
		de(yt,dyt);
		for( int i=0; i<n; i++ )
			y[i] += h6*( dydt[i] +dyt[i] +2.*dym[i] );
	}
};

#endif
