/***************************************************************************
                          cadp.cpp  -  plasticity rule according to Shouval et al
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
#include "cadp.h"
using std::endl;

double CaDP::sig( double x, double beta ) const
{
  return 1/(1+exp(-beta*x));
}

double CaDP::omega(double Ca) const
{
  return nu_0 +(nu_ltd-nu_0)*sig(Ca-.1e-6,80e6) +(nu_ltp-nu_ltd-nu_0)*sig(Ca-.46e-6,80e6);
}

double CaDP::eta(double Ca) const
{
  //return 1/( .1e3/(1e-3+pow(Ca*1e6,3))+1e3 );
  //return -nu_ltd*sig(Ca-.25e-6,80e6) +nu_ltp*sig(Ca-.46e-6,80e6);
  return 1e-3*sig( Ca-3e-7, 8e6 );
}

CaDP::CaDP(long numnodes, double deltat)
  :deltat(deltat), nodes(numnodes){
  nu = new double[nodes];
  Ca = new double[nodes];
  binding = new double[nodes];
}

CaDP::~CaDP(){
  if(synapoutf.is_open()) synapoutf.close();
  if(voutf.is_open()) voutf.close();
  if(caoutf.is_open()) caoutf.close();
  if(bindoutf.is_open()) bindoutf.close();
  if(nu) delete[] nu;
  if(Ca) delete[] Ca;
  if(binding) delete[] binding;
}

void CaDP::init(Istrm& inputf, int coupleid){
  inputf.validate("Nu",58); inputf >> nu_0;
  inputf.validate("LTD",58); inputf >> nu_ltd;
  inputf.validate("LTP",58); inputf >> nu_ltp;
  inputf.validate("B",58); inputf >> B;
  inputf.validate("N",58); inputf >> N;
  inputf.validate("rho",58); inputf >> rho;
  srand ( time(NULL) );
  for( int i=0; i<nodes; i++ ) {
    nu[i] = nu_0;
    Ca[i] = 0;
    binding[i] = 0;
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
  ss.str(""); ss<<"neurofield.bindout."<<coupleid;
  bindoutf.open(ss.str().c_str(),std::ios::out);
  if(!bindoutf){
    std::cerr<<"Unable to open 'neurofield.bindout."<<coupleid<<"' for output.\n";
    exit(EXIT_FAILURE);
  }
  this->coupleid = --coupleid;
}

void CaDP::dump(ofstream& dumpf){
  dumpf<<"Nu: "<<nu_0<<" ";
  dumpf<<"LTD: "<<nu_ltd<<" ";
  dumpf<<"LTP: "<<nu_ltp<<" ";
  dumpf<<"B: "<<B<<" ";
  dumpf<<"N: "<<N<<" ";
  dumpf<<"rho: "<<rho<<" ";
}

void CaDP::output(){
  for( int i=0; i</*nodes*/1; i++ ) {
    synapoutf<<setprecision(14)<<rho*nu[i]<<endl;
    caoutf<<setprecision(14)<<Ca[i]<<endl;
    voutf<<setprecision(14)<<V[i]<<endl;
    bindoutf<<setprecision(14)<<binding[i]<<endl;
  }
}

void CaDP::updatePa(double *Pa, double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat,Couplinglist& couplinglist){
  V = qhistorylist.getQhist(connectmat.getQindex(coupleid)).getVbytime(0);
  for( int i=0; i<nodes; i++ ) {
    binding[i] = 1;//sig( couplinglist.glu[i] -200e-6, B );
    double dCa = deltat*(2e-3*binding[i])*(195e-3-V[i])*sig( V[i]-45.5e-3-log(1e-0)/62,62 )
      -Ca[i]/50e-3*deltat;
    if( Ca[i]+dCa < 0 )
      Ca[i] = 0;
    else
      Ca[i] += dCa;
    double dnu = deltat*eta(Ca[i])*(omega(Ca[i])-nu[i]);
    if( sign*(nu[i]+dnu) < 0 )
      nu[i] = 0;
    else
      nu[i] += dnu;
    // Sum the coupling terms transforming Phi_{ab} to P_{ab}
    Pa[i]=nu[i]*Etaa[i];
  }
}
