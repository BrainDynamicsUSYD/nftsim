/***************************************************************************
                          waveeqn.cpp  -  Wave equation propagator
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "waveeqn.h"
#include<math.h>
#include<cstdlib>
#include<string>
using std::string;
#include"qhistory.h"

using std::endl;

WaveEqn::WaveEqn(long nodes, double dt, long longside):
                nodes(nodes),deltat(dt),longsidelength(longside){
  if (nodes%longsidelength != 0){
    std::cerr << "To define a rectangular grid nodes: " << nodes <<endl;
    std::cerr << "divided by Longside: " << longside << endl;
    std::cerr << "must have no remainder" << endl;
    exit(EXIT_FAILURE); 
  }
  shortsidelength=nodes/longsidelength;
  if(longsidelength<2 || shortsidelength<2){
    std::cerr << "Error: The shortest dimension in Waveeqnrect" << endl;
    std::cerr << "must be 2 or greater" << endl;
    exit(EXIT_FAILURE);
  }
  gridsize=(longsidelength+2)*(shortsidelength+2);
  startfirstrow=longsidelength+3;
  startlastrow=(longsidelength+2)*shortsidelength+1;
  phipast = new Field(gridsize,longsidelength,shortsidelength,"Phi");
  Qpast = new Field(gridsize,longsidelength,shortsidelength,"Q");
}

WaveEqn::~WaveEqn(){
  delete phipast;
  delete Qpast;
  if(tauobj) delete tauobj;
}

void WaveEqn::init(Istrm& inputf,Qhistory& qhistory){
  // Determine if an initial value is given or "Steady==0" initial condition
  double buffer; inputf.Param("Phi",buffer);
  if( buffer )
    phipast->init(buffer);
  else
    phipast->init(qhistory.getQbytime(0)[0]);

  /*inputf.Param("Initial Phi");
  std::streampos sp;
  sp = inputf.tellg();
  char ch;
  ch=inputf.get();
  while(' '==ch)ch=inputf.get();
  if('S'==ch){
    double* Q=qhistory.getQbytime(0);
    phipast->init(Q[0]);
  } else {
    inputf.seekg(sp);
    double initphi;
    inputf >> initphi;
    phipast->init(initphi);
  }*/
  inputf.Param("Deltax",deltax);
  tauobj = new Tau(nodes,deltat,inputf,qhistory);
  inputf.Param("Range",effrange);
  if( !inputf.Optional("gamma",gamma) ) {
    inputf.Param("velocity",gamma);
    gamma = gamma/effrange;
  }
  if(gamma/2.0 < deltat || effrange/2.0 < deltax){
    std::cerr << "Wave equation with gamma: " << gamma << " effrange: " << effrange << endl;
    std::cerr << "Is neither adequately captured by grid spacing chosen" << endl;
    std::cerr << "nor sufficiently localized so the potential can be approximated by Q" << endl;
    exit(EXIT_FAILURE);
  }
  if(gamma*effrange*deltat/deltax>1.41){
    std::cerr << "Wave equation with gamma: " << gamma << " effrange: " << effrange << endl;
    std::cerr << "and deltat: " << deltat << " deltax: " << deltax << endl;
    std::cerr << "does not fulfill the Courant condition" << endl;
    std::cerr << "Courant number is : " << (gamma*effrange*deltat/deltax) << endl;
    exit(EXIT_FAILURE);
  }
  double* Q=qhistory.getQbytime(*tauobj);
  Qpast->init(Q);//+rand());
  deltat2divided12=(deltat*deltat)/12.0F; //factor in wave equation
  deltatdivideddeltaxallsquared=(deltat*deltat)/(deltax*deltax);
  p2=deltatdivideddeltaxallsquared*(effrange*effrange*gamma*gamma); // Square of mesh ratio, dimensionless
}

void WaveEqn::dump(ofstream& dumpf){
  tauobj->dump(dumpf);
  dumpf << "Deltax: " << deltax << " ";
  dumpf << "Range: " << effrange << " ";
  dumpf << "gamma: " << gamma << " " << endl;
  phipast->dump(dumpf);
  Qpast->dump(dumpf);
}

void WaveEqn::restart(Istrm& restartf,Qhistory& qhistory){
  /*restartf.ignore(45); // Throw away everything up to the dash char
  tauobj = new Tau(nodes,deltat,restartf,qhistory);
  restartf.Param("Deltax",deltax);
  restartf.Param("Range",effrange);
  if( !restartf.Optional("gamma",gamma) ) {
    restartf.Param("velocity",gamma);
    gamma = gamma/effrange;
  }
  if( !((1==optionnum)||(2==optionnum)) ){
    std::cerr << "Last read looking for gamma or velocity found neither" << endl;
    exit(EXIT_FAILURE);
  }
  if(gamma*effrange*deltat/deltax>1.41){
    std::cerr << "Wave equation with gamma: " << gamma << " effrange: " << effrange << endl;
    std::cerr << "and deltat: " << deltat << " deltax: " << deltax << endl;
    std::cerr << "does not fulfill the Courant condition" << endl;
    std::cerr << "Courant number is : " << (gamma*effrange*deltat/deltax) << endl;
    exit(EXIT_FAILURE);
  }
  phipast->restart(restartf);
  Qpast->restart(restartf);
  deltat2divided12=(deltat*deltat)/12.0F; //factor in wave equation
  deltatdivideddeltaxallsquared=(deltat*deltat)/(deltax*deltax);
  p2=deltatdivideddeltaxallsquared*(effrange*effrange*gamma*gamma); // Square of mesh ratio, dimensionless*/
}

void WaveEqn::stepwaveeq(double *Phi,Qhistory& qhistory){
  double sumphi;
  double sumphidiag;
  double sumq;
  double sumqdiag;
  double drive;
  double* Phi_1= phipast->U_1;
  double* Phi_2= phipast->U_2;
  double* Q=qhistory.getQbytime(*tauobj);
  double* Q_1= Qpast->U_1;
  double* Q_2= Qpast->U_2;
  // gamma=gammaobj->get(); //Update the gamma value
  // effrange=effrangeobj->get(); //Update the effective range value
  // p2=deltatdivideddeltaxallsquared*(effrange*effrange*gamma*gamma); // Square of mesh ratio, dimensionless
//  twominusfourp2=2.0F-4.0F*p2; // factor in wave algorithm
  twominusthreep2=2.0F-3.0F*p2; // factor in wave algorithm
//  tenminusfourp2=10.0F-4*p2; //factor in wave algorithm
  tenminusthreep2=10.0F-3.0F*p2; //factor in wave algorithm
  dfact=deltat2divided12*gamma*gamma; // Factor of deltat^2 /gamma^2 / 12
  expfact1=exp(-1.0F*deltat*gamma);
  expfact2=exp(-2.0F*deltat*gamma);
  // initialize five indexes to spatial positions
  icentre=startfirstrow;
  itop=startfirstrow-(longsidelength+2);
  ibottom=startfirstrow+(longsidelength+2);
  ileft=startfirstrow-1;
  iright=startfirstrow+1;
  itopleft=itop-1;
  itopright=itop+1;
  ibottomleft=ibottom-1;
  ibottomright=ibottom+1;
  iphi=0;
  // loop over nodes
  for(long i=0; i<shortsidelength; i++){
    for(long j=0; j<longsidelength; j++){
      sumphi=Phi_1[itop]+Phi_1[ibottom]+Phi_1[ileft]+Phi_1[iright];
      sumphidiag=Phi_1[itopleft]+Phi_1[itopright]+Phi_1[ibottomleft]+Phi_1[ibottomright];
      sumq=Q_1[itop]+Q_1[ibottom]+Q_1[ileft]+Q_1[iright];
      sumqdiag=Q_1[itopleft]+Q_1[itopright]+Q_1[ibottomleft]+Q_1[ibottomright];
      drive=dfact*(tenminusthreep2*expfact1*Q_1[icentre]+Q[iphi]+expfact2*Q_2[icentre]
                 +expfact1*(0.5)*p2*(sumq+0.5*sumqdiag));
      Phi[iphi]=twominusthreep2*expfact1*Phi_1[icentre]
                 +expfact1*(0.5)*p2*(sumphi+0.5*sumphidiag)-expfact2*Phi_2[icentre];
      Phi[iphi]+=drive;
      icentre++,itop++,ibottom++,ileft++,iright++; // increment position indexes
      itopleft++,itopright++,ibottomleft++,ibottomright++; // as above
      iphi++; // increment phi position index
    }
    // reposition indexes to start of next row,
    // they were already incremented one space within inner loop
    icentre+=2,itop+=2,ibottom+=2,ileft+=2,iright+=2;
    itopleft+=2,itopright+=2,ibottomleft+=2,ibottomright+=2;
  }
  phipast->update(Phi); // Store current phi value in past value arrays for next step
  Qpast->update(Q); // Store current Q value in past value array for next step of PDE
}
