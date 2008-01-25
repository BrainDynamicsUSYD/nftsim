/***************************************************************************
                          gamma.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "gamma.h"

Gamma::Gamma(){
}
Gamma::~Gamma(){
}

void Gamma::init(Istrm& inputf){
  inputf.validate("gamma",58);
  inputf >> gamma;
}

void Gamma::dump(ofstream& dumpf){
  dumpf << "gamma:" << gamma << " ";
}

void Gamma::restart(Istrm& restartf){
  restartf.validate("gamma",58);
  restartf >> gamma;
}

float Gamma::get(){
  return gamma;
}
