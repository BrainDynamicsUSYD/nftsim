/***************************************************************************
                          cadp.cpp  -  plasticity rule according to Shouval et al
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
#include "cadp.h"
using std::endl;

CaDP::CaDP(long numnodes, double deltat)
  :nodes(numnodes){
}

CaDP::~CaDP(){
  if(synapoutf.is_open()) synapoutf.close();
}

void CaDP::init(Istrm& inputf, int coupleid){
  double asdf; //dummy variable
  inputf.validate("Initial nu",58); inputf >> asdf;
  inputf.validate("rho",58); inputf >> asdf;
  inputf.validate("alpha",58); inputf >> asdf;
  inputf.validate("beta",58); inputf >> asdf;
  inputf.validate("gamma",58); inputf >> asdf;
  inputf.validate("Ap",58); inputf >> asdf;
  inputf.validate("Am",58); inputf >> asdf;
  inputf.validate("Taup",58); inputf >> asdf;
  inputf.validate("Taum",58); inputf >> asdf;
  inputf.validate("B",58); inputf >> asdf;
  inputf.validate("N",58); inputf >> asdf;
  inputf.validate("Q_max",58); inputf >> asdf;

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

void CaDP::dump(ofstream& dumpf){
}

void CaDP::output(){
  synapoutf<<setprecision(14)<<*V<<endl;
}

void CaDP::updatePa(double *Pa, double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist){
  V = qhistorylist.getQhist(connectmat.getQindex(coupleid)).getVbytime(0);
  double nu = 0; // dummy line to compile code
  // Sum the coupling terms transforming Phi_{ab} to P_{ab}
  long n=nodes;
  for(int i=0; i<n; i++)
    Pa[i]=nu*Etaa[i];
}
