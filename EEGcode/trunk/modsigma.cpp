/***************************************************************************
                          modsigma.cpp  -  variance dynamics model for modulating
                                           sigma
                             -------------------
   copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<math.h>
#include<string>
#include<cstdlib>
using std::string;
#include<sstream>
using std::stringstream;
#include "modsigma.h"

Modsigma::Modsigma(Istrm& inputf, int popindex){
  double temp;
  inputf.validate("SigmaTheta",58);
  inputf >> temp;
  sigmatheta2=temp*temp;
  inputf.validate("Initial SigmaV",58);
  inputf >> sigmaVpast;
  inputf.validate("Av. length",58);
  inputf >> avlength;
// 
  initoutput(inputf,popindex);
}

Modsigma::~Modsigma(){
}

void Modsigma::dump(ofstream& dumpf){
  dumpf << " SigmaTheta: " << sqrt(sigmatheta2);
  dumpf << " Initial SigmaV: " << sigmaVpast;
  dumpf << " Av. length: " << avlength;
}

double Modsigma::get(double timestep, double* Vab){
  double sigma;
  sigmaVpast=exp(-timestep/avlength)*sigmaVpast+Vab[1];
  sigma= sqrt(sigmatheta2+sigmaVpast*sigmaVpast);
  output();
  return (sigma);
}

void Modsigma::initoutput(Istrm& inputf, int popindex){
  popindex++; // (popindex+1) is to ensure numbering from one not zero in output
  stringstream ss(stringstream::in | stringstream::out);
  ss << "eegcode.sigmaout." << popindex;
  sigmaoutf.open(ss.str().c_str(),ios::out);
  if( !sigmaoutf ){
    std::cerr << "Unable to open 'eegcode.sigmaout." << popindex << "' for output \n";
    exit(EXIT_FAILURE);
  }
  sigmaoutf << "Robinson variance dynamics model output data" << endl;
}

void Modsigma::output(){
  sigmaoutf << "SigmaV: " << sigmaVpast << " ";
}
