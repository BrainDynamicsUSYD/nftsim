#include<cstdlib>
#include<cmath>
#include"dendriteramp.h"

void DendriteRamp::DendriteDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y = {dV/dt, V, nuphi}
  dydt[0] = ( y[3] - 1 - (1/alpha + 1/beta) * y[1]) * (alpha * beta);
  // V
  dydt[1] = y[1]; 															
  // d(nuphi)/dt from precouple
  dydt[2] = 0;
}

void DendriteRamp::init( Configf& configf )
{
  Dendrite::init(configf);

  //configf.param("alpha1",alpha1);
  //configf.param("beta1",beta1);
  configf.param("alpha2",alpha2);
  configf.param("beta2",beta2);
  configf.param("t1",t1);
  configf.param("t2",t2);
  configf.param("t3",t3);
  configf.param("t4",t4);
  time = 0;
  de->init(v[0]); // call DendriteRamp::DendriteDE::init
  de->alpha = alpha1;
  de->beta = beta1;
}

void DendriteRamp::DendriteDE::init(const double vinit)
{
  variables[0].clear(); variables[1].clear(); variables[2].clear(); 
  variables[0].resize(nodes, 0.0); variables[1].resize(nodes, vinit); 
  variables[2].resize(nodes, 0.0); 
}

DendriteRamp::DendriteRamp( int nodes, double deltat, int index,
    const Propag& prepropag, const Couple& precouple )
  : Dendrite(nodes,deltat,index,prepropag,precouple)
    
{
  de = new DendriteDE(nodes, deltat);
  rk4 = new RK4(*de);
}

DendriteRamp::~DendriteRamp(void)
{
  delete de;
  delete rk4;
}

void DendriteRamp::step(void)
{
  for( int i=0; i<nodes; i++ )
    (*de)[2][i] = precouple[i];
  time += deltat;
  // alpha and beta already initialized 
  if (time > t1 && time <= t2)				// ramp alpha, beta up
  {
    de->alpha += de->alpha+deltat*alpha1;
	de->beta += de->beta+deltat*beta1;
	//alpha[i] =  alpha1 + (time - t1) * (alpha2 - alpha1) / (t2 - t1);
	//beta[i] = beta1 + (time -t1) * (beta2 - beta1) / (t2 -t1);
  }
 // else if (time > de->t2 && time <= de->t3) 			// alpha == alpha_0 + K
 // {  
 //   de->alpha = de->alpha2;
 //	de->beta = de->beta2;
 //  }
 // else if (time > de->t3 && time <= de->t4) 				// ramp nu down
 // {
 // 	de->alpha =  de->alpha2 - ((time - de->t3) * (de->alpha2 - de->alpha1) / (de->t4 - de->t3));
 //   de->beta = de->beta2 - ((time - de->t3) * (de->beta2 - de->beta1) /(de->t4 - de->t3));
 // }
 else if (time > t4) 								// nu == nu_0
 {
    de->alpha = alpha1;
	de->beta = beta1;
 }
 rk4->step();
}

void DendriteRamp::output( Output& output ) const
{
  output("Dendrite",index+1,"V",V());
  //output.singleNode("Dendrite",index+1,"alpha",de->alpha); // single node hasn't been implemented
  //output.singleNode("Dendrite",index+1,"beta",de->beta);
}
