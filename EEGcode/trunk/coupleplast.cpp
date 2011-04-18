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
}

void Coupleplast::output(){
  synapoutf<<setprecision(14)<<nu<<endl;
}

//
// Sum the coupling terms transforming Phi_{ab} to P_{ab}
//
void Coupleplast::updatePa(double *Pa, double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist){
  complex<double> detA[int(W_CUTOFF/W_STEP)];
  for( int i=0; i<int(W_CUTOFF/W_STEP); i++ ) {
    detA[i] = 1;
	Couple* couple = NULL;
	for( int j=0; couple == couplinglist.getcoup(j); j++ ) {
      couple = couplinglist.getcoup(j);
      detA[i] -= couple->X(i);
    }
  }

  //double result = 0;
  //for( int i=0; i<=(x1-x0)/dx; i++ )
    //result += f( x0+i*dx );
  //return result*dx;

  long n=nodes;
  for(int i=0; i<n; i++)
    Pa[i]=nu*Etaa[i];
}

complex<double> Coupleplast::X( int i ) const
{
  return rho*nu *L[i]*Gamma[i];
  //for( int i=0; i<int(W_CUTOFF/W_STEP); i++ )
    //return_val[i] -= rho*nu *L[i]*Gamma[i];
}
