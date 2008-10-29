/***************************************************************************
                          waveeqn.cpp  -  Wave equation propagator
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "waveeqn.h"
#include<math.h>

WaveEqn::WaveEqn(long nodes, double dt):gammaobj("gamma"),
           effrangeobj("Effective range"),deltat(dt){
  gridsize=static_cast<long>((sqrt(nodes)+2)*(sqrt(nodes)+2));
  if (sqrt( static_cast<double>(nodes)) != floor(sqrt( static_cast<double>(nodes)))){
    cerr << "Wave equation solver assumes square grid. Nodes per population must be a perfect square number" << endl;
    exit(EXIT_FAILURE);
  }
  rowlength=static_cast<long>(sqrt(gridsize));
  longsidelength=rowlength-2;
  shortsidelength=rowlength-2;
  startfirstrow=longsidelength+3;
  startlastrow=(longsidelength+2)*shortsidelength+1;
  phipast = new Field(gridsize,longsidelength,shortsidelength,"Phi");
  Qpast = new Field(gridsize,longsidelength,shortsidelength,"Q");
}

WaveEqn::WaveEqn(long nodes, double dt, long longside):gammaobj("gamma"),
           effrangeobj("Effective range"),deltat(dt),longsidelength(longside){
  if (nodes%longsidelength != 0){
    cerr << "To define a rectangular grid nodes: " << nodes <<endl;
    cerr << "divided by Longside: " << longside << endl;
    cerr << "must have no remainder" << endl;
    exit(EXIT_FAILURE); 
  }
  shortsidelength=nodes/longsidelength;
  if(longsidelength<2 || shortsidelength<2){
    cerr << "Error: The shortest dimension in Waveeqnrect" << endl;
    cerr << "must be 2 or greater" << endl;
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
}

void WaveEqn::init(Istrm& inputf, Qhistory* pqhistory){
  phipast->init(inputf);
  inputf.validate("Deltax",58);
  inputf >> deltax;
  inputf.validate("Tauab",58);
  float tauabfloat;
  inputf >> tauabfloat;
  tauab=int(tauabfloat);
  if(tauabfloat<1 && tauabfloat>0){
    cerr << "Last read Tauab: " << tauabfloat << endl;
    cerr << "Tauab must be greater than 1 as it is measured in" << endl;
    cerr << "time steps not a time measured in seconds" << endl;
    exit(EXIT_FAILURE);
  }
  effrangeobj.init(inputf);
  gammaobj.init(inputf);
  gamma=gammaobj.get(); //Update the gamma value
  effrange=effrangeobj.get(); //Update the effective range value
  if(gamma/2.0 < deltat || effrange/2.0 < deltax){
    cerr << "Wave equation with gamma: " << gamma << " effrange: " << effrange << endl;
    cerr << "Is neither adequately captured by grid spacing chosen" << endl;
    cerr << "nor sufficiently localized so the potential can be approximated by Q" << endl;
    exit(EXIT_FAILURE);
  }
  double* Q= pqhistory->getQbytime(tauab);
  Qpast->init(Q);
  deltat2divided12=(deltat*deltat)/12.0F; //factor in wave equation
  deltatdivideddeltaxallsquared=(deltat*deltat)/(deltax*deltax);
}

void WaveEqn::dump(ofstream& dumpf){
  dumpf << "Tau_ab: " << tauab << " ";
  dumpf << "Deltax: " << deltax << " ";
  effrangeobj.dump(dumpf);
  gammaobj.dump(dumpf);
  dumpf << endl;
  phipast->dump(dumpf);
  Qpast->dump(dumpf);
}

void WaveEqn::restart(Istrm& restartf){
  restartf.ignore(200,58); // throw away everything before the colon character
  restartf >> tauab;
  restartf.validate("Deltax",58);
  restartf >> deltax;
  effrangeobj.restart(restartf);
  gammaobj.restart(restartf);
  phipast->restart(restartf);
  Qpast->restart(restartf);
  deltat2divided12=(deltat*deltat)/12.0F; //factor in wave equation
  deltatdivideddeltaxallsquared=(deltat*deltat)/(deltax*deltax);
}

void WaveEqn::stepwaveeq(double *Phi, Qhistory *pqhistory){
  double sumphi;
  double sumphidiag;
  double sumq;
  double sumqdiag;
  double drive;
  double* Phi_1= phipast->U_1;
  double* Phi_2= phipast->U_2;
  double* Q= pqhistory->getQbytime(tauab);
  double* Q_1= Qpast->U_1;
  double* Q_2= Qpast->U_2;
  gamma=gammaobj.get(); //Update the gamma value
  effrange=effrangeobj.get(); //Update the effective range value
  p2=deltatdivideddeltaxallsquared*(effrange*effrange*gamma*gamma)  ; // Square of mesh ratio, dimensionless
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
    icentre+=2,itop+=2,ibottom+=2,ileft+=2,iright+=2; // reposition indexes to start of next row, they were already incremented
        					      // one space within inner loop
    itopleft+=2,itopright+=2,ibottomleft+=2,ibottomright+=2; // as above
  }
  phipast->update(Phi); // Store current phi value in past value arrays for next step
  Qpast->update(Q); // Store current Q value in past value array for next step of PDE
}
