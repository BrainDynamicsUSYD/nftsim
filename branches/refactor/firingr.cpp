/***************************************************************************
                          firingr.cpp  -  Compute firing response

                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "firingr.h"
#include<cmath>
using std::endl;

FiringR::FiringR(int popindex,Istrm& inputf){
  if( inputf.Optional("Theta",theta) ) {
    inputf.Param("Sigma",sigma);
    inputf.Param("Qmax",Q_max);
  }
  else {
    inputf.Param("Gradient",gradient);
    inputf.Param("Intercept",intercept);
  }
}

FiringR::~FiringR(){
}

// Method to transform V into Q via sigmoid firing response
void FiringR::getQ(double *V,double *Q,long nodes,double timestep){
  if(theta)
    for( long i=0; i<nodes; i++ )
      Q[i] = Q_max/( 1.0F+ exp( -(V[i]-theta)/sigma ) );
  else
    for( long i=0; i<nodes; i++ )
      Q[i] = V[i]*gradient +intercept;
}

void FiringR::dump(ofstream& dumpf){
  if(theta)
    dumpf << "Sigmoid - Theta: " << theta
      << " Sigma: " << sigma << " Qmax: " << Q_max << endl;
  else
    dumpf << "Linear - Gradient: " << gradient
      << " Intercept: " << intercept << endl;
}
