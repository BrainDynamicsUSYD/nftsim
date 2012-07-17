#include<iostream>
using std::cout;
using std::endl;
#include<cmath>
#include"de.h"

struct Sine : public DE
{
	Sine(double deltat) : DE(2,deltat) {}
	virtual void define( const vector<double>& y, vector<double>& dydt ) {
		dydt[0] = y[1];
		dydt[1] = -y[0];
	}
};

int main(void)
{
	double pi = 3.141592654;
	double steps = 1000;
	// initiate sine DE
	Sine sine(2*pi/steps);
	sine.y[0]    = 0; sine.y[1]    = 1;
	sine.dydt[0] = 1; sine.dydt[1] = 0;
	// evaluate sine function
	for( double t=0; t<2*pi; t+=2*pi/steps ) {
		cout<<t<<"\t"<<sine.y[0]<<endl;
		sine.rk4();
	}
	return 0;
};
