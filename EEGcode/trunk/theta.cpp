/***************************************************************************
                          theta.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "theta.h"

Theta::Theta(){
}
Theta::~Theta(){
}

void Theta::init(Istrm& inputf){
  inputf.validate("Theta",58);
  inputf >> theta;
}

void Theta::dump(ofstream& dumpf){
  dumpf << "Theta:" << theta <<" ";
}

void Theta::restart(Istrm& restartf){
  restartf.validate("Theta",58);
  restartf >> theta;
}

float Theta::get(){
  return theta;
}
