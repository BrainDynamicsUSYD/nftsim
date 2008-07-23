/***************************************************************************
                          weqn.cpp  -  description
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "weqn.h"
#include<math.h>

Weqn::Weqn(long gsize, double dt):gammaobj("gamma"),
           effrangeobj("Effective range"),gridsize(gsize), 
	   deltat(dt){
  rowlength=static_cast<long>(sqrt(gridsize));
  longsidelength=rowlength-2;
  shortsidelength=rowlength-2;
  startfirstrow=longsidelength+3;
  startlastrow=(longsidelength+2)*shortsidelength+1;
  Qpast = new Field(gridsize,"Q");
}

Weqn::~Weqn(){
  delete Qpast;
}

void Weqn::init(Istrm& inputf, double deltax, Qhistory* pqhistory){
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

void Weqn::dump(ofstream& dumpf){
  dumpf << "Tau_ab: " << tauab << " ";
  effrangeobj.dump(dumpf);
  gammaobj.dump(dumpf);
  dumpf << endl;
  Qpast->dump(dumpf);
}

void Weqn::restart(Istrm& restartf, double deltax){
  restartf.ignore(200,58); // throw away everything before the colon character
  restartf >> tauab;
  effrangeobj.restart(restartf);
  gammaobj.restart(restartf);
  restartf.ignore(200,32); // throw away endl
  Qpast->restart(restartf);
  deltat2divided12=(deltat*deltat)/12.0F; //factor in wave equation
  deltatdivideddeltaxallsquared=(deltat*deltat)/(deltax*deltax);
}

void Weqn::stepwaveeq(double *PhiRe, double *PhiIm, Qhistory *pqhistory, Field* fieldReobj, Field* fieldImobj, Prefact* prefactobj){
  double sumphiRe, sumphiIm;
  double sumphiRediag,sumphiImdiag;
  double sumqRe, sumqIm;
  double sumqRediag,sumqImdiag;
  double driveRe, driveIm;
  double* Phi_1Re= fieldReobj->U_1;
  double* Phi_1Im= fieldImobj->U_1;
  double* Phi_2Re= fieldReobj->U_2;
  double* Phi_2Im= fieldImobj->U_2;
  double* factRe= prefactobj->factRe;
  double* factIm= prefactobj->factIm;
  double* Q= pqhistory->getQbytime(tauab);
  double* Q_1= Qpast->U_1;
  double* Q_2= Qpast->U_2;
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
      sumphiRe=Phi_1Re[itop]+Phi_1Re[ibottom]+Phi_1Re[ileft]+Phi_1Re[iright];
      sumphiIm=Phi_1Im[itop]+Phi_1Im[ibottom]+Phi_1Im[ileft]+Phi_1Im[iright];
      sumphiRediag=Phi_1Re[itopleft]+Phi_1Re[itopright]+Phi_1Re[ibottomleft]+Phi_1Re[ibottomright];
      sumphiImdiag=Phi_1Im[itopleft]+Phi_1Im[itopright]+Phi_1Im[ibottomleft]+Phi_1Im[ibottomright];
      sumqRe=factRe[itop]*Q_1[itop]+factRe[ibottom]*Q_1[ibottom]
	         +factRe[ileft]*Q_1[ileft]+factRe[iright]*Q_1[iright];
      sumqIm=factIm[itop]*Q_1[itop]+factIm[ibottom]*Q_1[ibottom]
	         +factIm[ileft]*Q_1[ileft]+factIm[iright]*Q_1[iright];
      sumqRediag=factRe[itopleft]*Q_1[itopleft]+factRe[itopright]*Q_1[itopright]
	         +factRe[ibottomleft]*Q_1[ibottomleft]+factRe[ibottomright]*Q_1[ibottomright];
      sumqImdiag=factIm[itopleft]*Q_1[itopleft]+factIm[itopright]*Q_1[itopright]
	         +factIm[ibottomleft]*Q_1[ibottomleft]+factIm[ibottomright]*Q_1[ibottomright];
      driveRe=dfact*( p2*expfact1*(0.5)*(sumqRe+0.5*sumqRediag) + factRe[icentre]*
	      (tenminusfourp2*expfact1*Q_1[icentre]+Q[iphi]+expfact2*Q_2[icentre]) );
      driveIm=dfact*( p2*expfact1*(0.5)*(sumqIm+0.5*sumqImdiag) + factIm[icentre]*
	      (tenminusfourp2*expfact1*Q_1[icentre]+Q[iphi]+expfact2*Q_2[icentre]) );
      PhiRe[iphi]=twominusfourp2*expfact1*Phi_1Re[icentre]
             +p2*expfact1*(0.5)*(sumphiRe+0.5*sumphiRediag)-expfact2*Phi_2Re[icentre];
      PhiIm[iphi]=twominusfourp2*expfact1*Phi_1Im[icentre]
             +p2*expfact1*(0.5)*(sumphiIm+0.5*sumphiImdiag)-expfact2*Phi_2Im[icentre];
      PhiRe[iphi]+=driveRe;
      PhiIm[iphi]+=driveIm;
      icentre++,itop++,ibottom++,ileft++,iright++; // increment position indexes
      itopleft++,itopright++,ibottomleft++,ibottomright++; // as above
      iphi++; // increment phi position index
    }
    icentre+=2,itop+=2,ibottom+=2,ileft+=2,iright+=2; // reposition indexes to start of next row, they were already incremented
        					      // one space within inner loop
    itopleft+=2,itopright+=2,ibottomleft+=2,ibottomright+=2; // as above
  }
  Qpast->update(Q); // Store current Q value in past value array for next step of PDE
}
