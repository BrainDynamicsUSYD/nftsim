/***************************************************************************
                          waveeqn.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "waveeqn.h"
#include<math.h>

WaveEqn::WaveEqn(long gsize, float dt):gammaobj("gamma"),
           effrangeobj("Effective range"),gridsize(gsize), 
	   deltat(dt){
  rowlength=static_cast<long>(sqrt(gridsize));
  sidelength=rowlength-2;
  startfirstrow=rowlength+1;
  startlastrow=rowlength*sidelength+1;
  Phi_1 = new float[gridsize];
  Phi_2 = new float[gridsize];
}


WaveEqn::~WaveEqn(){
  delete[ ] Phi_1;
  delete[ ] Phi_2;
}

void WaveEqn::init(Istrm& inputf){
  inputf.validate("Initial Phi",58);
  float Phi_initial;
  inputf >> Phi_initial;
  for(long i=0; i<gridsize; i++){
    Phi_1[i]=Phi_initial;
    Phi_2[i]=Phi_initial;
  }
  inputf.validate("Deltax",58);
  inputf >> deltax;
  inputf.validate("Tauab",58);
  inputf >> tauab;
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
  deltat2divided12=(deltat*deltat)/12.0F; //factor in wave equation
  deltatdivideddeltaxallsquared=(deltat*deltat)/(deltax*deltax);
}

void WaveEqn::dump(ofstream& dumpf){
  dumpf << "Tau_ab: " << tauab << " ";
  dumpf << "Deltax: " << deltax << " ";
  effrangeobj.dump(dumpf);
  gammaobj.dump(dumpf);
  dumpf << endl;
  dumpf << "Phi_1: " ;
  for(long i=0; i<gridsize; i++)
    dumpf << Phi_1[i] << " ";
  dumpf << endl;
  dumpf << "Phi_2: " ;
  for(long i=0; i<gridsize; i++)
    dumpf << Phi_2[i] << " ";
  dumpf << endl;
}

void WaveEqn::restart(Istrm& restartf){
  restartf.ignore(200,58); // throw away everything before the colon character
  restartf >> tauab;
  restartf.validate("Deltax",58);
  restartf >> deltax;
  effrangeobj.restart(restartf);
  gammaobj.restart(restartf);
  restartf.ignore(200,58); //throw away endl and then Phi_1: 
  for(long i=0; i<gridsize; i++)
    restartf >> Phi_1[i];
  restartf.ignore(200,58); //throw away endl and the Phi_2:
  for(long i=0; i<gridsize; i++)
    restartf >> Phi_2[i];
  restartf.ignore(200,32); // throw away endl
}

void WaveEqn::stepwaveeq(float *Phi, Qhistory *pqhistory){
  float sumphi;
  float sumq;
  float drive;
  float* Q= pqhistory->getQbytime(tauab);
  float* Q_1= pqhistory->getQbytime(tauab+1);
  float* Q_2= pqhistory->getQbytime(tauab+2);
  gamma=gammaobj.get(); //Update the gamma value
  effrange=effrangeobj.get(); //Update the effective range value
  p2=deltatdivideddeltaxallsquared*(effrange*effrange*gamma*gamma)  ; // Square of mesh ratio, dimensionless
  twominusfourp2=2.0F-4.0F*p2; // factor in wave algorithm
  tenminusfourp2=10.0F-4*p2; //factor in wave algorithm
  dfact=deltat2divided12*gamma*gamma; // Factor of deltat^2 /gamma^2 / 12
  expfact1=exp(-1.0F*deltat*gamma);
  expfact2=exp(-2.0F*deltat*gamma);
  // initialize five indexes to spatial positions
  icentre=startfirstrow;
  itop=startfirstrow-rowlength;
  ibottom=startfirstrow+rowlength;
  ileft=startfirstrow-1;
  iright=startfirstrow+1;
  iphi=0;
  // loop over nodes
  for(long i=0; i<sidelength; i++){
    for(long j=0; j<sidelength; j++){
      sumphi=Phi_1[itop]+Phi_1[ibottom]+Phi_1[ileft]+Phi_1[iright];
      sumq=Q_1[icentre]+Q_1[ibottom]+Q_1[ileft]+Q_1[iright];
      drive=dfact*(tenminusfourp2*expfact1*Q_1[icentre]+Q[icentre]+expfact2*Q_2[icentre]+ p2*expfact1*sumq);
      Phi[iphi]=twominusfourp2*expfact1*Phi_1[icentre]+p2*expfact1*sumphi-expfact2*Phi_2[icentre];
      Phi[iphi]+=drive;
      icentre++,itop++,ibottom++,ileft++,iright++; // increment position indexes
      iphi++; // increment phi position index
    }
    icentre+=2,itop+=2,ibottom+=2,ileft+=2,iright+=2; // reposition indexes to start of next row, they were already incremented
        					      // one space within inner loop
  }
  //
  // Copy Phi_1[i] back to Phi_2[i]
  //
  for(long i=0; i<gridsize; i++)
    Phi_2[i]=Phi_1[i];
  //
  // Copy Phi[i] to Phi_1[i] taking into account array size differences
  //
  float * pPhi=Phi; // Get pointer to start of Phi array
  float * pPhi_1=Phi_1; // Get pointer to start of Phi_1 array
// Next part copies middle of Phi values grid across
  float * pp1=pPhi_1+startfirstrow;
  float * pp=pPhi;
  for(long i=0; i<sidelength;i++){
    for(long j=0; j<sidelength;j++){
      *pp1++=*pp++;
    }
    pp1+=2; // reposition at start of next row
  }
// Next part copies top line based on last row
  pp1=pPhi_1+1;
  pp=pPhi+ sidelength*(sidelength-1);
  for(long i=0; i<sidelength;i++)
    *pp1++=*pp++;
// Next part copies bottom line based on first row
  pp1=pPhi_1+startlastrow+rowlength;
  pp=pPhi;
  for(long i=0; i<sidelength;i++)
    *pp1++=*pp++;
// Next part copies left line based on last column 
  pp1=pPhi_1+startfirstrow-1;
  pp=pPhi+sidelength-1;
  for(long i=0; i<sidelength;i++){
   *pp1=*pp;
   pp1+=rowlength;
   pp+=sidelength;
  }
// Next part copies right line based on first column 
  pp1=pPhi_1+startfirstrow+sidelength;
  pp=pPhi;
  for(long i=0; i<sidelength;i++){
    *pp1=*pp;
    pp1+=rowlength;
    pp+=sidelength;
  }
}
