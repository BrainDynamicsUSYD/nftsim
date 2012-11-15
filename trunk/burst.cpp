#include"burst.h"

void BurstResponse::init( Configf& configf )
{
    configf.param("Sigma",sigma);
    configf.param("Qmax",Q_max);
    double hinit; configf.param("Htilde",hinit);
    double xinit; configf.param("Xtilde",xinit);
    for(size_t i = 0; i<dendrites.size();i++)
        configf>>*dendrites[i];
 // read config file- see qresponse.init()
 // initialize any other private variables (i.e. current) 
 // using variables from Robinson, Wu, Kim 07

    htilde.resize(nodes,hinit);
    xtilde.resize(nodes,xinit);
    modtheta.resize(nodes,0);
    thetatemp.resize(nodes,0);
    qfiring.resize(nodes,0);
    xinfinity.resize(nodes,0);

    xk.resize(4);
    hk.resize(4);
    for(int i=0; i<4;i++){
        xk[i].resize(nodes,0);
        hk[i].resize(nodes,0);
    }
    // k[0] to k[3] is k-values for xtilde
    // ditto for htilde k[4] to k[7]
        
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
	: QResponse(nodes,deltat,index), xtemp(nodes), htemp(nodes)
{
    ia=-2.6; //A m^-2
    ib=1.2; // A m^-2
    ic=0.197; // A m^-2
    taux=0.015; // 15ms 
    tauh=0.056; // 56ms
    ax=0.26; // for tauR=2.1ms
    mu=3.1; // mu=-Itheta/theta S m^-2
    
    h=deltat;
    h2=deltat/2;
    h6=deltat/6;
}

BurstResponse::~BurstResponse(void)
{
}

void BurstResponse::step(void)
{
  // happens once per timestep
  QResponse::step(); // sum soma potential
  rk4(); // update dynamic variables in y
  for(int i=0; i<nodes; i++ )
    modtheta[i] = (3*ib*xtilde[i]+(ib-ia)*htilde[i])/mu; // use updated y to calculate theta

}

void BurstResponse::output( Output& output ) const
{
  // write a field into an output file
  // see cadp.cpp
  output.prefix("Burst",index+1);
  output("Htilde",htilde);
  output("Xtilde",xtilde);
  output("modtheta",modtheta);
}

void BurstResponse::fire( vector<double>& Q ) const
{
    for(int i=0; i<nodes; i++ )
        Q[i] = Q_max/( 1.0F+ exp( -(v[i]-modtheta[i])/sigma ) );
}
  
void BurstResponse::rk4() 
{
    // Takes current values of htilde and xtilda
    // Updates them by one time step in place
        
	rkderivs(xtilde,htilde,xk[0],hk[0]);
	for(int i=0; i<nodes; i++){
	    xtemp[i] = xtilde[i] + h2*xk[0][i];
	    htemp[i] = htilde[i] + h2*hk[0][i];
	}
   
	rkderivs(xtemp,htemp,xk[1],hk[1]);
	for(int i=0; i<nodes; i++){
	    xtemp[i] = xtilde[i] + h2*xk[1][i];
	    htemp[i] = htilde[i] + h2*hk[1][i];
	}
	
	rkderivs(xtemp,htemp,xk[2],hk[2]);
	for(int i=0; i<nodes; i++){
	    xtemp[i] = xtilde[i] + h*xk[2][i];
	    htemp[i] = htilde[i] + h*hk[2][i];
	}
	    
	rkderivs(xtemp,htemp,xk[3],hk[3]);
	
	for(int i=0; i<nodes; i++){
	    xtilde[i] = xtilde[i] + h6*(xk[0][i] + 2*xk[1][i] + 2*xk[2][i] + xk[3][i]);
        htilde[i] = htilde[i] + h6*(hk[0][i] + 2*hk[1][i] + 2*hk[2][i] + hk[3][i]);
    }
}

void BurstResponse::rkderivs(vector<double>& xtemp, vector<double>& htemp,vector<double>& xk, vector<double>& hk)
{
	//function returns the derivatives for modulated threshold eqns
	//y[0] is Htilde , y[1] is Xtilde
	for(int i = 0; i < nodes; i++){
	    thetatemp[i] = (ic-3*ib*xtilde[i]+(ib-ia)*htilde[i])/mu;
	    qfiring[i] = Q_max/(1+exp(-(v[i]-thetatemp[i])/sigma));
	    
	    xinfinity[i] = qfiring[i]*ax;
	    if(xinfinity[i] < 0)
	        xinfinity[i] = 0;
	        
	    hk[i] = (3*xtemp[i]-htemp[i])/tauh;
	    xk[i] = (xinfinity[i] - xtemp[i])/taux;
	}
}
