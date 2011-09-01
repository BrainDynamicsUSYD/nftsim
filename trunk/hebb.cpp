/***************************************************************************
                          hebb.cpp  -  simple Hebbian plasticity rule
                             -------------------
   copyright            : (C) 2009 
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include <math.h>
#include<cstdlib>
#include<ctime>
#include<string>
using std::string;
#include<sstream>
using std::stringstream;
#include<iomanip>
using std::setprecision;
#include "hebb.h"
using std::endl;

Hebb::Hebb(long numnodes, double deltat)
  :deltat(deltat), nodes(numnodes){
  nu = new double[nodes];
}

Hebb::~Hebb(){
  if(synapoutf.is_open()) synapoutf.close();
  if(nu) delete[] nu;
}

void Hebb::init(Istrm& inputf, int coupleid){
  double init_nu;
  inputf.validate("Initial nu",58); inputf >> init_nu;
  inputf.validate("N",58); inputf >> N;
  inputf.validate("rho",58); inputf >> rho;
  inputf.validate("B",58); inputf >> B;
  srand ( time(NULL) );
  for( int i=0; i<nodes; i++ ) {
    nu[i] = init_nu;// +( double(rand())/double(RAND_MAX))*init_nu/10 );
  }
  sign = int(nu[0]/fabs(nu[0]));

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

void Hebb::dump(ofstream& dumpf){
  dumpf<<"Initial nu: "<<nu[0]<<" ";
  dumpf<<"N: "<<N<<" ";
  dumpf<<"rho: "<<rho<<" ";
  dumpf<<"B: "<<B<<" ";
}

void Hebb::output(){
  for( int i=0; i</*nodes*/1; i++ ) {
    synapoutf<<setprecision(14)<<rho*nu[i]<<endl;
  }
}

void Hebb::updatePa(double *Pa, double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist){
  double* Q = qhistorylist.getQhist(connectmat.getQindex(coupleid)).getQbytime(0);
  for( int i=0; i<nodes; i++ ) {
    double dnu = deltat*B*Etaa[i]*Q[i] -nu[i]/200e-3;
    if( sign*(nu[i]+dnu) < 0 )
      nu[i] = 0;
    else
      nu[i] += dnu;
    // Sum the coupling terms transforming Phi_{ab} to P_{ab}
    Pa[i]=nu[i]*Etaa[i];
  }
}
