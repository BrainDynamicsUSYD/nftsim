#ifndef DIFFEQN
#define DIFFEQN

#include<vector>
using std::vector;

class DE
{
	DE(void);
	DE(DE&);
	void operator=(DE&);
protected:
	int n; // dimension of system == y.size() == dydt.size()
	double deltat;
	double h6; // == deltat/6

	vector<double> k1;
	vector<double> k2;
	vector<double> k3;
	vector<double> k4;
	vector<double> temp;

	// define dydt here
	virtual void define( const vector<double>& y, vector<double>& dydt ) = 0;
public:
	vector<double> y;
	vector<double> dydt;

	DE( int n, double deltat )
		: n(n), deltat(deltat), h6(deltat/6.),
		  y(n), dydt(n), k1(n), k2(n), k3(n), k4(n), temp(n) {}
	virtual ~DE(void) {}

	void rk4(void) {
		define(y      ,k1);
		for( int i=0; i<n; i++ )
			temp[i] = y[i] +deltat*.5*k1[i];
		define(temp,k2);
		for( int i=0; i<n; i++ )
			temp[i] = y[i] +deltat*.5*k2[i];
		define(temp,k3);
		for( int i=0; i<n; i++ )
			temp[i] = y[i] +deltat*k3[i];
		define(temp,k4);
		for( int i=0; i<n; i++ )
			y[i] += h6*( k1[i] +2*k2[i] +2*k3[i] +k4[i] );
	}
};

#endif
