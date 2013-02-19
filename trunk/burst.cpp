#include"burst.h"

void BurstResponse::init( Configf& configf )
{
    string noob; configf.param("Mode",noob);
    configf.param("Sigma",sigma);
    configf.param("Qmax",Q_max);
    double hinit; configf.param("Htilde",hinit);
    double xinit; configf.param("Xtilde",xinit);
    configf.param("ia",ia);
    configf.param("ib",ib);
    configf.param("ic",ic);
    configf.param("taux",taux);
    configf.param("tauh",tauh);
    configf.param("ax",ax);
    configf.param("mu",mu);

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
    modtheta[i] = (ic-3*ib*xtilde[i]+(ib-ia)*htilde[i])/mu; // use updated y to calculate theta

}

void BurstResponse::output(Output& output) const
{
  // write a field into an output file
  // see cadp.cpp
  output.prefix("Population",index+1);
  output("HTilde",htilde);
  output("XTilde",xtilde);
  output("modtheta",modtheta);
  output("V",v);
}

void BurstResponse::outputDendrite(int index, Output& output) const
{
       // if(qresponse) return qresponse->outputDendrite(index,output);
}
void BurstResponse::fire( vector<double>& Q ) const
{
    for(int i=0; i<nodes; i++ )
    {    //   if (v[i]+ic/mu-modtheta[i] >0.0129)
         // Q[i] = 86 * sqrt((v[i]+ic/mu-modtheta[i])/0.0129 -1);
         Q[i] = Q_max/( 1.0F+ exp( -(v[i]-modtheta[i])/sigma ) );

      //else
      //    Q[i] = 0;
    }      
}
  
void BurstResponse::rk4() 
{
    // Takes current values of htilde and xtilde
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
        //if (v[i]+ic/mu-thetatemp[i] >0.0129)
          //qfiring[i] = 86 * sqrt((v[i]+ic/mu-thetatemp[i])/0.0129 -1);
        //else
        //   qfiring[i] = 0;

	    xinfinity[i] = qfiring[i]*ax;
	    if(xinfinity[i] < 0)
	        xinfinity[i] = 0;
	        
	    hk[i] = (3*xtemp[i]-htemp[i])/tauh;
	    xk[i] = (xinfinity[i] - xtemp[i])/taux;
	}
}
