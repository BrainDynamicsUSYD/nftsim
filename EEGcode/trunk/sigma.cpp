/***************************************************************************
                          sigma.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "sigma.h"

Sigma::Sigma(){
}
Sigma::~Sigma(){
}

void Sigma::init(Istrm& inputf){
  inputf.validate("Sigma",58);
  inputf >> sigma;
}

void Sigma::dump(ofstream& dumpf){
  dumpf << "Sigma:" << sigma <<" ";
}

void Sigma::restart(Istrm& restartf){
  restartf.validate("Sigma",58);
  restartf >> sigma;
}

float Sigma::get(){
  return sigma;
}
