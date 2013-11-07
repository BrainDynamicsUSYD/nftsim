#include<cstdlib>
#include<iostream>
using std::endl;
#include<cmath>
#include"dendritedynamic.h"

void DendriteDynamic::init( Configf& configf )
{
  configf.param("alpha", alpha); 
  configf.param("alpha2", alpha2); 
  configf.param("beta", beta); 
  configf.param("beta2", beta2); 
  configf.param("t1",t1);
  configf.param("t2",t2);
  configf.param("t3",t3);
  configf.param("t4",t4);
}

DendriteDynamic::DendriteDynamic( int nodes, double deltat, int index,
    const Propag& prepropag, const Couple& precouple )
  : Dendrite(nodes,deltat,index,prepropag,precouple)
{
  time = 0;
}

DendriteDynamic::~DendriteDynamic(void)
{
}

void DendriteDynamic::step(void)
{
  time+=deltat;
  if (time <= t1)
  { // Do nothing here!
  } // find what are the relevant vectors for alpha and beta!!!!
  else if (time > t1 && time <= t2)				// ramp alpha, beta up
	for( int i=0; i<nodes; i++ )
		n[i] =  nu1 + (time - t1) * (nu2 - nu1) / (t2 - t1);
  else if (time > t2 && time <= t3) 				// nu == nu_0 + K
	for( int i=0; i<nodes; i++ )
		n[i] = nu2;
  else if (time >t3 && time <= t4) 				// ramp nu down
	for( int i=0; i<nodes; i++ )	
		n[i] =  nu2 - ((time - t3) * (nu2 - nu1) / (t4 - t3));
 else if (time > t4) 						// nu == nu_0
	for( int i=0; i<nodes; i++ )
		n[i] = nu1;
 for( int i=0; i<nodes; i++ )
	P[i] = n[i]*prepropag[i];
}

void DendriteDynamic::output( Output& output )
{
  // DE* de; RK4 rk4(*de);
  output("alpha",(*de)[0]);
  output("beta",(*de)[1]);
  output("V",(*de)[2]);
}
