/***************************************************************************
                          weqn.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "weqn.h"
#include<math.h>

Weqn::Weqn(long gsize, double dt):gammaobj("gamma"),
           effrangeobj("Effective range"),gridsize(gsize), 
	   deltat(dt){
  rowlength=static_cast<long>(sqrt(gridsize));
  sidelength=rowlength-2;
  startfirstrow=rowlength+1;
  startlastrow=rowlength*sidelength+1;
}

Weqn::~Weqn(){}

void Weqn::init(Istrm& inputf, double deltax){
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

void Weqn::dump(ofstream& dumpf){
  dumpf << "Tau_ab: " << tauab << " ";
  effrangeobj.dump(dumpf);
  gammaobj.dump(dumpf);
  dumpf << endl;
}

void Weqn::restart(Istrm& restartf, double deltax){
  restartf.ignore(200,58); // throw away everything before the colon character
  restartf >> tauab;
  effrangeobj.restart(restartf);
  gammaobj.restart(restartf);
  restartf.ignore(200,32); // throw away endl
  deltat2divided12=(deltat*deltat)/12.0F; //factor in wave equation
  deltatdivideddeltaxallsquared=(deltat*deltat)/(deltax*deltax);
}

void Weqn::stepwaveeq(double *PhiRe, double *PhiIm, Qhistory *pqhistory, Wavefield* fieldobj, Prefact* prefactobj){
  double sumphiRe, sumphiIm;
  double sumqRe, sumqIm;
  double driveRe, driveIm;
  double* Phi_1Re= fieldobj->U_1Re;
  double* Phi_1Im= fieldobj->U_1Im;
  double* Phi_2Re= fieldobj->U_2Re;
  double* Phi_2Im= fieldobj->U_2Im;
  double* factRe= prefactobj->factRe;
  double* factIm= prefactobj->factIm;
  double* Q= pqhistory->getQbytime(tauab);
  double* Q_1= pqhistory->getQbytime(tauab+1);
  double* Q_2= pqhistory->getQbytime(tauab+2);
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
      sumphiRe=Phi_1Re[itop]+Phi_1Re[ibottom]+Phi_1Re[ileft]+Phi_1Re[iright];
      sumphiIm=Phi_1Im[itop]+Phi_1Im[ibottom]+Phi_1Im[ileft]+Phi_1Im[iright];
      sumqRe=factRe[itop]*Q_1[itop]+factRe[ibottom]*Q_1[ibottom]
	         +factRe[ileft]*Q_1[ileft]+factRe[iright]*Q_1[iright];
      sumqIm=factIm[itop]*Q_1[itop]+factIm[ibottom]*Q_1[ibottom]
	         +factIm[ileft]*Q_1[ileft]+factIm[iright]*Q_1[iright];
      driveRe=dfact*( p2*expfact1*sumqRe + factRe[icentre]*
	      (tenminusfourp2*expfact1*Q_1[icentre]+Q[icentre]+expfact2*Q_2[icentre]) );
      driveIm=dfact*( p2*expfact1*sumqIm + factIm[icentre]*
	      (tenminusfourp2*expfact1*Q_1[icentre]+Q[icentre]+expfact2*Q_2[icentre]) );
      PhiRe[iphi]=twominusfourp2*expfact1*Phi_1Re[icentre]+p2*expfact1*sumphiRe-expfact2*Phi_2Re[icentre];
      PhiIm[iphi]=twominusfourp2*expfact1*Phi_1Im[icentre]+p2*expfact1*sumphiIm-expfact2*Phi_2Im[icentre];
      PhiRe[iphi]+=driveRe;
      PhiIm[iphi]+=driveIm;
      icentre++,itop++,ibottom++,ileft++,iright++; // increment position indexes
      iphi++; // increment phi position index
    }
    icentre+=2,itop+=2,ibottom+=2,ileft+=2,iright+=2; // reposition indexes to start of next row, they were already incremented
        					      // one space within inner loop
  }
}
