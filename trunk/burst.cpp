#include"burst.h"

void BurstResponse::init( Configf& configf )
{
    configf.param("Theta",theta);
    configf.param("Sigma",sigma);
    configf.param("Qmax",Q_max);
    configf>>dendrites;
 // read config file- see qresponse.init()
 // initialize any other private variables (i.e. current) 
 // using variables from Robinson, Wu, Kim 07
    Ia=-2.6; //A m^-2
    Ib=1.2; // A m^-2
    Ic=0.197; // A m^-2
    taux=0.015; // 15ms 
    tauh=0.056; // 56ms
    Ax=0.26; // for tauR=2.1ms
    Mu=3.1; // mu=-Itheta/theta S m^-2
    initXtilde=0.0;
    initHtilde=0.0;
    y[0]=initXtilde;
    y[1]=initHtilde;
    h=deltat;
    h2=deltat/2;
    h6=deltat/6;
}

void BurstResponse::restart( Restartf& restartf )
{
  // leave empty
}

void BurstResponse::dump( Dumpf& dumpf ) const
{
  // leave empty
}

BurstResponse::BurstResponse( int nodes, double deltat, int index )
	: QResponse(nodes,deltat,index)
{
  // leave empty
}

BurstResponse::~BurstResponse(void)
{
}

void BurstResponse::step(void)
{
  // happens once per timestep
  QResponse::step(); // sum soma potential
  rk4(); // update dynamic variables in y
  //itheta=Ic-3*ib*y[1]+(Ib-Ia)*y[0];? 
  theta = (3*Ib*y[1]+(Ib-Ia)*y[0])/Mu; // use updated y to calculate theta

}

void BurstResponse::fire( vector<double>& Q ) const
{
  if(theta)
	  for( int i=0; i<nodes; i++ )
	    Q[i]=Q_max/(1.0F+ exp( -(v[i]-theta)/sigma ) ); // map voltage into
			// firing rate 1.0F is constant literal
			// uses updated theta from step()
}

const vector<double>& BurstResponse::V(void) const
{
  return v; // leave empty if you like, it just returns v
}

vector<Output*> BurstResponse::output(void) const
{
  // write a field into an output file
  // see cadp.cpp
  vector<Output*> temp;
  // Something like this- check with Felix how to do it properly (throws error)
  temp.push_back( new Output( label("Burst.",index+1)+".Htilde", y[0] ) ); 
  temp.push_back( new Output( label("Burst.",index+1)+".Xtilde", y[1] ) ); 
  return temp;
}

void BurstResponse::rk4() {

	rkderivs(y,k1);
	for(int i=0; i<2; i++) 
	    yt[i]=y[i]+h2*k1[i];
	rkderivs(yt,k2);
	for(int i=0; i<2; i++) 
	    yt[i]=y[i]+h2*k2[i];
	rkderivs(yt,k3);
	for(int i=0; i<2; i++)
	    yt[i]=y[i]+h*k3[i];
	rkderivs(yt,k4);
	for(int i=0; i<2; i++) 
	    y[i]=y[i]+h6*(k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
}

void BurstResponse::rkderivs(double* yt,double* dydt){
	//function returns the derivatives for modulated threshold eqns
	//y[0] is Htilde , y[1] is Xtilde
	double thetatemp=(Ic-3*Ib*y[1]+(Ib-Ia)*y[0])/Mu;
	double Qfiring=Q_max/(1+exp(-(v[1]-thetatemp)/sigma));
	double xinfinity;
	
	
	if((Qfiring*Ax)>0) {xinfinity =Ax*Qfiring;}
	else {xinfinity=0;}
	dydt[0]=(3*y[1]-y[0])/tauh; // Htilde DE
	dydt[1]=(xinfinity-y[1])/taux; // Xtilde DE
}
