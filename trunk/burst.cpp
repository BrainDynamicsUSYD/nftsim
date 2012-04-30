#include"burst.h"

void BurstResponse::init( Configf& inputf )
{
  if( configf.optional("Theta",theta) )) {
	  configf.param(("Sigma",sigma);
	  configf.param("Qmax",Q_max);
  }
  configf>>dendrites;
}// read config file- see qresponse.init()
 // initialize any other private variables (i.e. current)
Ia
Ib
Ic
Taux
Tauh
Ax
Mu
Xtilda
Htilda

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
  // leave empty
}

void BurstResponse::step(void)
{
  // happens once per timestep. calculates V by summing soma potentials
    dendrites.step();
  for( int i=0; i<nodes; i++ )
    v[i] = 0;
  for( size_t i=0; i<dendrites.size(); i++ )
    for( int j=0; j<nodes; j++ )
      v[j] += dendrites[i]->V()[j];
  // also perform an rk4 step to update the current variables
  this->rk4();
}

void BurstResponse::fire( vector<double>& Q ) const
{
  if(theta)
	  for( int i=0; i<nodes; i++ )
	    Q[i]=Q_max/(1.0F+ exp( -(v[i]-theta)/sigma ) ) // map voltage into firing rate
}

const vector<double>& BurstResponse::V(void) const
{
  return v; // leave empty if you like, it just returns v
}

vector<Output*> BurstResponse::output(void) const
{
  // write stuff into an output file
  // see cadp.cpp
  vector<Output*> temp;
  temp.push_back( new Output( label("Burst.",index+1)+".ic", ic ) ); // ic not defined yet
  return vector<Output*>(1,temp);
}

rk4(double t,double* y, float timestep, double* V, double Qmax, double sigma) {
	// Write an rk4 function here
	double yt[2];
	double dyt[2];
	double dym[2];
	double h=timestep;
	double h2=timestep/2;
	double h6=timestep/6;

	rkderivs(t,y,dydt,V,Qmax,sigma);
	for(int i=0; i<2; i++) yt[i]=y[i]+h2*dydt[i];
	rkderivs(t+h2,yt,dyt,V,Qmax,sigma);
	for(int i=0; i<2; i++) yt[i]=y[i]+h2*dyt[i];
	rkderivs(t+h2,yt,dym,V,Qmax,sigma);
	for(int i=0; i<2; i++) {yt[i]=y[i]+h*dym[i]; dym[i]+=dyt[i];}
	rkderivs(t+h,yt,dyt,V,Qmax,sigma);
	for(int i=0; i<2;; i++) y[i]=y[i]+h6*(dydt[i]+dyt[i]+2*dym[i]);
}

void rkderivs(double steptime, double* y, double* dydt, double* V, double Qmax,
		double sigma){
	//function returns the derivatives for modulated threshold eqns
	//y[0] is Htilda , y[1] is Xtilda

	double thetatemp=(Ic-3*Ib*y[1]+(Ib-Ia)*y[0])/mu;
	double Qfiring=Qmax/(1+exp(-(V[1]-thetatemp)/sigma));
	double xinfinity;
	if((Qfiring*Ax)>0) {xinfinity =Ax*Qfiring;}
	else {xinfinity=0;}
	dydt[0]=(3*y[1]-y[0])/tauh; // Htilde DE
	dydt[1]=(xinfinity-y[1])/taux; // Xtilde DE
}
