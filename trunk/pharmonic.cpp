/***************************************************************************
                          pharmonic.cpp  -  propagator with harmonic oscillators
			                    and no spatial coupling
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "pharmonic.h"
#include<math.h>
#include"qhistory.h"

using std::endl;

Pharmonic::Pharmonic(long nodes, double dt):nodes(nodes),
            timestep(dt){
  previousQ = new double [nodes];
  previousPhi = new double [nodes];
  dPhidt = new double [nodes];
}

Pharmonic::~Pharmonic(){
  delete[ ] previousQ;
  delete[ ] previousPhi;
  delete[ ] dPhidt;
  if(tauobj) delete tauobj;
}

void Pharmonic::init(Istrm& inputf,Qhistory& qhistory){
  double phiinit;

  inputf.Param("Initial Phi",phiinit);
  for(long i=0; i<nodes; i++){
    previousQ[i]=phiinit;
    previousPhi[i]=phiinit;
    dPhidt[i]=0.;
  }
  tauobj = new Tau(nodes,timestep,inputf,qhistory);
  inputf.Param("gamma",gamma);
}

void Pharmonic::dump(ofstream& dumpf){
  tauobj->dump(dumpf);
  dumpf << "gamma: " << gamma << endl;
  dumpf << "Q_previous:";
  for(long i=0; i<nodes; i++){
    dumpf << previousQ[i] << " ";
  }
  dumpf << endl;
  dumpf << "Phi_previous:";
  for(long i=0; i<nodes; i++){
    dumpf << previousPhi[i] << " ";
  }
  dumpf << endl;
  dumpf << "dPhidt:";
  for(long i=0; i<nodes; i++){
    dumpf << dPhidt[i] << " ";
  }
  dumpf << endl; // Add endline to propagator input
}

void Pharmonic::restart(Istrm& restartf,Qhistory& qhistory){
  restartf.ignore(200,45);
  tauobj = new Tau(nodes,timestep,restartf,qhistory);
  restartf.Param("gamma",gamma);
  /*double temp;
  restartf.validate("Q_previous",58);
  for(long i=0; i<nodes; i++){
    restartf >> temp;
    previousQ[i]=temp;
  }
  restartf.validate("Phi_previous",58);
  for(long i=0; i<nodes; i++){
    restartf >> temp;
    previousPhi[i]=temp;
  }
  restartf.validate("dPhidt",58);
  for(long i=0; i<nodes; i++){
    restartf >> temp;
    dPhidt[i]=temp;
  }*/
}

void Pharmonic::stepwaveeq(double * Phi,Qhistory& qhistory){
//
//  The program currently assumes that Gamma is constant and Q(t) is linear for the time step
//  This is since it is very costly to obtain Q(t).
//  Under these assumptions the solution can be explicitly obtained.
//  Calculating the explicit solution is computationally faster than using
//  Runge-Kutta to evaluate the time step.
//  At current time alpha and beta are not functions of space and so 
//  computed variables are used to speed up the calculation.
//
  double adjustedQ;
  double factorgamma;
  double deltaQdeltat;
  double C1;
  double C1deltatplusc2;

  double* Q=qhistory.getQbytime(*tauobj);
  double expgamma=exp(-gamma*timestep);
  factorgamma=(2.0/gamma);
  for(long i=0; i<nodes; i++){
    deltaQdeltat=(Q[i]-previousQ[i])/timestep;
    adjustedQ=previousQ[i]-factorgamma*deltaQdeltat-previousPhi[i];
    C1=dPhidt[i]-gamma*adjustedQ-deltaQdeltat;
    C1deltatplusc2=C1*timestep-adjustedQ;
    Phi[i]=C1deltatplusc2*expgamma+Q[i]-factorgamma*deltaQdeltat;
    dPhidt[i]=(C1-gamma*C1deltatplusc2)*expgamma+deltaQdeltat;
    previousQ[i]=Q[i]; //Save current Q for next step
    previousPhi[i]=Phi[i]; //Save current Q for next step
  }
}
