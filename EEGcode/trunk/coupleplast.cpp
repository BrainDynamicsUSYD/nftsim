/***************************************************************************
                          coupleplast.cpp  -  Implements a neuronal plasticity
                                              model of Robinson (2009)
                             -------------------
   copyright            : (C) 2009 
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include <math.h>
#include<string>
using std::string;
#include<sstream>
using std::stringstream;
#include<iomanip>
using std::setprecision;
#include "coupleplast.h"
using std::endl;
using std::complex;

Coupleplast::Coupleplast(long numnodes, double deltat)
  :nodes(numnodes), i0(0.,1.), c1(1.,0.){
}

Coupleplast::~Coupleplast(){
  if(synapoutf.is_open()) synapoutf.close();
}

void Coupleplast::init(Istrm& inputf, int coupleid){
  inputf.validate("Initial nu",58); inputf >> nu;
  inputf.validate("rho",58); inputf >> rho;
  inputf.validate("alpha",58); inputf >> alpha;
  inputf.validate("beta",58); inputf >> beta;
  inputf.validate("gamma",58); inputf >> gamma;
  inputf.validate("Ap",58); inputf >> Ap;
  inputf.validate("Am",58); inputf >> Am;
  inputf.validate("Taup",58); inputf >> Taup;
  inputf.validate("Taum",58); inputf >> Taum;
  inputf.validate("B",58); inputf >> B;
  inputf.validate("N",58); inputf >> N;
  inputf.validate("Q_max",58); inputf >> Q_max;

  sign = nu/fabs(nu);

  for( int i=0; i<int(W_CUTOFF/W_STEP); i++ ) {
    double w = i*W_STEP;
	L[i] = c1/(c1-i0*w/alpha)/(c1-i0*w/beta);
    H[i] = Ap*Taup/(c1-i0*w*Taup) + Am*Taum/(c1+i0*w*Taum);
    Gamma[i] = c1/pow(c1-i0*w/gamma,2);
    filter[i] = pow(abs(L[i]),2)*real(conj(H[i])*conj(Gamma[i]));
  }

  coupleid++;
  stringstream ss(stringstream::in|stringstream::out);
  ss<<"neurofield.synaptout."<<coupleid;
  synapoutf.open(ss.str().c_str(),std::ios::out);
  if(!synapoutf){
    std::cerr<<"Unable to open 'neurofield.synaptout."<<coupleid<<"' for output.\n";
    exit(EXIT_FAILURE);
  }
  this->coupleid = --coupleid;
}

void Coupleplast::dump(ofstream& dumpf){
  dumpf << "nu: " << nu << " ";
  dumpf << "rho: " << rho << " ";
  dumpf << "alpha: " << alpha << " ";
  dumpf << "beta: " << beta << " ";
  dumpf << "gamma: " << gamma << " ";
  dumpf << "Ap: " << Ap << " ";
  dumpf << "Am: " << Am << " ";
  dumpf << "Taup: " << Taup << " ";
  dumpf << "Taum: " << Taum << " ";
  dumpf << "B: " << B << " ";
  dumpf << "N: " << N << " ";
  dumpf << "Q_max: " << Q_max << " ";
}

void Coupleplast::output(){
  // output G rather than nu, so that it is simplier to inspect G_e+G_i
  synapoutf<<setprecision(14)<<rho*nu<<endl;
}

void Coupleplast::updatePa(double *Pa, double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist){
  // Find dnudt as the Riemann sum of filter/detA
  double dnudt = 0;
  for( int i=0; i<int(W_CUTOFF/W_STEP); i++ ) {
    complex<double> detA = 1;
    Couple* couple = NULL;
    for( int j=0; couple != couplinglist.getcoup(j); j++ ) {
      couple = couplinglist.getcoup(j);
      detA -= couple->X(i); // detA(w) = 1 - Sum_a X_a for a=e,i
    }
    dnudt += filter[i]/pow( abs(detA), 2 );
  }
  double Q = *(qhistorylist.getQhist(connectmat.getQindex(coupleid)).getQbytime(0)); // get the instantaneous firing rate of this population
  dnudt *= N*B*W_STEP/2/3.14 *(1-Q/Q_max);
  if( fabs(dnudt)>fabs(nu) && dnudt/fabs(dnudt)!=sign )
    nu = 0; // the coupling constants do not cross nu=0
  else
    nu += dnudt;

  // Sum the coupling terms transforming Phi_{ab} to P_{ab}
  long n=nodes;
  for(int i=0; i<n; i++)
    Pa[i]=nu*Etaa[i];
}

complex<double> Coupleplast::X( int i ) const
{
  return rho*nu *L[i]*Gamma[i];
}
