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

double sig( double x, double beta )
{
  double expo = exp(beta*x);
  return expo/(1+expo);
}

double omega(double Ca)
{
  double alpha0 = 0.33333; double alpha1 = 0.22e-6; double alpha2 = 0.39e-6;
  double beta1 = 80e6; double beta2 = 40e6;
  return ( alpha0 -alpha0*sig(Ca-alpha1,beta1) +sig(Ca-alpha2,beta2) );
}

double eta(double Ca)
{
  double p1 = 1; double p2 = 0.28e-6; double p3 = 3; double p4 = 1e-11;
  double power = pow(Ca+p4,p3);
  return p1*power/( power + pow(p2,p3) );
}

CaDP::CaDP(long numnodes, double deltat)
  :deltat(deltat), nodes(numnodes){
  nu = new double[nodes];
  Ca = new double[nodes];
}

CaDP::~CaDP(){
  if(synapoutf.is_open()) synapoutf.close();
  if(voutf.is_open()) voutf.close();
  if(caoutf.is_open()) caoutf.close();
  if(nu) delete[] nu;
  if(Ca) delete[] Ca;
}

void CaDP::init(Istrm& inputf, int coupleid){
  double init_nu; double init_Ca;
  inputf.validate("Initial nu",58); inputf >> init_nu;
  inputf.validate("Initial Ca",58); inputf >> init_Ca;
  inputf.validate("N",58); inputf >> N;
  inputf.validate("rho",58); inputf >> rho;
  inputf.validate("NMDA",58); inputf >> nmda;
  inputf.validate("V_r",58); inputf >> V_r;
  inputf.validate("tCa",58); inputf >> tCa;
  inputf.validate("B",58); inputf >> B;
  inputf.validate("scale",58); inputf >> scale;
  for( int i=0; i<nodes; i++ ) {
    nu[i] = init_nu;
    Ca[i] = init_Ca;
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
  ss.str(""); ss<<"neurofield.caout."<<coupleid;
  caoutf.open(ss.str().c_str(),std::ios::out);
  if(!caoutf){
    std::cerr<<"Unable to open 'neurofield.caout."<<coupleid<<"' for output.\n";
    exit(EXIT_FAILURE);
  }
  ss.str(""); ss<<"neurofield.vout."<<coupleid;
  voutf.open(ss.str().c_str(),std::ios::out);
  if(!voutf){
    std::cerr<<"Unable to open 'neurofield.vout."<<coupleid<<"' for output.\n";
    exit(EXIT_FAILURE);
  }
  this->coupleid = --coupleid;
}

void CaDP::dump(ofstream& dumpf){
  dumpf<<"Initial nu: "<<nu[0]<<" ";
  dumpf<<"Initial Ca: "<<Ca[0]<<" ";
  dumpf<<"N: "<<N<<" ";
  dumpf<<"rho: "<<rho<<" ";
  dumpf<<"NMDA: "<<nmda<<" ";
  dumpf<<"tCa: "<<nmda<<" ";
  dumpf<<"B: "<<nmda<<" ";
  dumpf<<"scale: "<<nmda<<" ";
}

void CaDP::output(){
  synapoutf<<setprecision(14)<<rho*nu[0]<<endl;
  caoutf<<setprecision(14)<<Ca[0]<<endl;
  voutf<<setprecision(14)<<V[0]<<endl;
}

void CaDP::updatePa(double *Pa, double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist){
// parameter values that are currently ad hoc:
// V_r, 1/3 in binding
  double Mg = 1e+0;

  V = qhistorylist.getQhist(connectmat.getQindex(coupleid)).getVbytime(0);
  //std::cout<<"CoupleID "<<coupleid<<" Q Index "<<connectmat.getQindex(coupleid)<<" QHistory object "<<&qhistorylist.getQhist(connectmat.getQindex(coupleid))<<" V "<<V[0]<<" "<<V[1]<<endl;
  for( int i=0; i<nodes; i++ ) {
    double v = V[i] +50e-3; // calibrated voltage
    double binding = sig( Etaa[i]*1e-7 - 1e-5, 1/3 ); // note 1/3 needs revision
    double dCadt = nmda*binding*(v-V_r) /(1+ exp(-0.062e3*v)*Mg/3.57 )
      -Ca[i]/tCa;
    if( dCadt<0 && fabs(dCadt)*deltat>Ca[i] )
      Ca[i] = 0;
    else
      Ca[i] += deltat*dCadt;
    double dnudt = B*eta(Ca[i])*(N*scale*omega(Ca[i])-nu[i]);
    if( fabs(dnudt)>fabs(nu[i]) && dnudt/fabs(dnudt)!=sign )
      nu[i] = 0;
    else
      nu[i] += sign*deltat*dnudt;
    // Sum the coupling terms transforming Phi_{ab} to P_{ab}
    Pa[i]=nu[i]*Etaa[i];
  }
}
