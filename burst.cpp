#include"burst.h"

void BurstResponse::init( Configf& configf )
{
    configf.param("Sigma",sigma);
    configf.param("Qmax",Q_max);
    double hinit; configf.param("Htilde",hinit);
    double xinit; configf.param("Xtilde",xinit);

    Veff = -0.066;	// from Matlab scripts
    Vx = 0.14;
    Vk = -0.095;

    double temp;
    configf.param("gX",temp);
    gX.push_back(temp);	
    configf.param("gH",temp);
    gH.push_back(temp);

    gX1 = gX[0]; 
    gH1 = gH[0];

    configf.param("ic",ic);
    configf.param("taux",taux);
    configf.param("tauh",tauh);
    configf.param("ax",ax);
    configf.param("mu",mu);

    if (configf.optional("gX2",gX2))
   {    
        configf.optional("tx1",tx1);
        configf.optional("tx2",tx2);
        configf.optional("tx3",tx3);
        configf.optional("tx4",tx4);
    }
    else
    {
        gX2 = gX1;
    }
    if (configf.optional("gH2",gH2))
    {   configf.optional("th1",th1);
        configf.optional("th2",th2);
        configf.optional("th3",th3);
        configf.optional("th4",th4);
    }
    else
    {
        gH2 = gH1;
    }

    for(size_t i = 0; i<dendrites.size();i++)
        configf>>*dendrites[i];	// array.h vector<T*> vector of ptrs info and inserts into this from the ifstream configf
 // read config file- see qresponse.init()
 // initialize any other private variables (i.e. current) 

    htilde.resize(nodes,hinit);
    xtilde.resize(nodes,xinit);
    modtheta.resize(nodes,0);
    thetatemp.resize(nodes,0);
    qfiring.resize(nodes,0);
    xinfinity.resize(nodes,0);

    xk.resize(4);	// vector<vector<double>> 
    hk.resize(4);
    for(int i=0; i<4;i++){
        xk[i].resize(nodes,0);
        hk[i].resize(nodes,0);
    }
}

BurstResponse::BurstResponse( int nodes, double deltat, int index )	// constructor
	: QResponse(nodes,deltat,index), xtemp(nodes), htemp(nodes)
{
    h=deltat;
    h2=deltat/2;
    h6=deltat/6;
    time = 0;
}

BurstResponse::~BurstResponse(void)
{
}

void BurstResponse::step(void)
{
  time++;
  double sec_time = time * deltat;
  if (sec_time <= tx1 || gX2 == gX1)
  { // Do nothing here!
  }
 	else if (sec_time > tx1 && sec_time <= tx2)				
			gX[0] =  gX1 + (sec_time - tx1) * (gX2 - gX1) / (tx2 - tx1);
 	else if (sec_time > tx2 && sec_time <= tx3) 				
			gX[0] = gX2;
 	else if (sec_time >tx3 && sec_time <= tx4) 				
			gX[0] =  gX2 - ((sec_time - tx3) * (gX2 - gX1) / (tx4 - tx3));
	else if (sec_time > tx4) 						
			gX[0] = gX1;

  if (sec_time <= th1 || gH2 == gH1)
  { // Do nothing here!
  }
 	else if (sec_time > tx1 && sec_time <= tx2)				
			gH[0] =  gH1 + (sec_time - th1) * (gH2 - gH1) / (th2 - th1);
 	else if (sec_time > th2 && sec_time <= th3) 				
			gH[0] = gH2;
 	else if (sec_time >th3 && sec_time <= th4) 				
			gH[0] =  gH2 - ((sec_time - th3) * (gH2 - gH1) / (th4 - th3));
	else if (sec_time > th4) 						
			gH[0] = gH1;

  QResponse::step(); // sum soma potential
  rk4(); // update dynamic variables in y
  ib = -gX[0] * (Veff -Vx) / 3;
  ia = ib - gH[0] * (Veff - Vk);  

  for(int i=0; i<nodes; i++ ){
    modtheta[i] = (ic-3*ib*xtilde[i]+(ib-ia)*htilde[i])/mu; // use updated y to calculate theta
  }
}

void BurstResponse::output(Output& output) const
{
  // write a field into an output file
  output.prefix("Population",index+1);
  output("HTilde",htilde);
  output("XTilde",xtilde);
  output("modtheta",modtheta);
  output("V",v);
  output("gX",gX);
  output("gH",gH);
}

void BurstResponse::outputDendrite(int index, Output& output) const
{
       // if(qresponse) return qresponse->outputDendrite(index,output);
}
void BurstResponse::fire( vector<double>& Q ) const
{
    for(int i=0; i<nodes; i++ )
    {    
      Q[i] = Q_max/( 1.0F+ exp( -(v[i]-modtheta[i])/sigma ) );
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
	ib = -gX[0] * (Veff -Vx) / 3;
	ia = ib - gH[0] * (Veff - Vk);
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
