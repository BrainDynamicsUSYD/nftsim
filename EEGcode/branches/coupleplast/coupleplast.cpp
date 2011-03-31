/***************************************************************************
                          coupleplast.cpp  -  Implements a neuronal plasticity
                                              model of Robinson (2009)
                             -------------------
   copyright            : (C) 2009 
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include <math.h>
#include <complex>
#include<string>
using std::string;
#include<sstream>
using std::stringstream;
#include<iomanip>
using std::setprecision;
#include "coupleplast.h"
#include "rect.h"
using std::endl;
using namespace std;

Coupleplast::Coupleplast(long numnodes, double deltat)
  :t(0),nodes(numnodes),timestep(deltat), i0(0.,1.), c1(1.,0.){
}

Coupleplast::~Coupleplast(){
  //delete alphaobj;
  //delete betaobj;
  if(synapoutf.is_open()) synapoutf.close();
}

void Coupleplast::init(Istrm& inputf, int coupleid){
  //alphaobj = new Parameter("Alpha",inputf);
  //betaobj = new Parameter("Beta",inputf);
  Parameter* nuobj = new Parameter("Nu",inputf);
  nu = nuobj->get();
  delete nuobj;
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
  dumpf << "Nu:" << nu << " ";
  //alphaobj->dump(dumpf);
  //betaobj->dump(dumpf);
}

void Coupleplast::output(){
  synapoutf<<setprecision(14)<<nu<<endl;
}

double Coupleplast::get(void) const
{
  return nu;
}

//
// Sum the coupling terms transforming Phi_{ab} to P_{ab}
//
void Coupleplast::updatePa(double *Pa, double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat){
  long n=nodes;
  rect Int;
  nu += Int.Int( 0, 5000, 1, &f );
  for(int i=0; i<n; i++)
    Pa[i]=nu*Etaa[i];
}

double Coupleplast::f( double w )
{
  //double G[2] = nuobj->get()*4200;
  double G_e = 0;//G[0];
  double G_i = 0;//G[1];
  double alpha = 75; double beta = 285;
  static complex<double> L = c1/(c1-i0*w/alpha)/(c1-i0*w/beta);
  double Aplus = 1; double Aminus = -0.4; double tauplus = 0.02; double tauminus = 0.04;
  static complex<double> H = Aplus*tauplus/(c1-i0*w*tauplus)
        + Aminus*tauminus/(c1+i0*w*tauminus);
  double gamma_e = 116; double gamma_i = 1500;
  static complex<double> Gamma_e = c1/pow(c1-i0*w/gamma_e,2);
  static complex<double> Gamma_i = c1/pow(c1-i0*w/gamma_i,2);
  static double constant = pow(abs(L),2)*real(conj(H)*conj(Gamma_e));
  complex<double> detA = c1 -L*Gamma_e*G_e -L*Gamma_i*G_i;
  return pow(abs(c1/detA),2)*constant;
}
