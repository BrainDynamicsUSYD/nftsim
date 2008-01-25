/***************************************************************************
                          effrange.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "effrange.h"

EffRange::EffRange(){
}
EffRange::~EffRange(){
}

void EffRange::init(Istrm& inputf){
  inputf.validate("Effective range",58);
  inputf >> effrange;
}

void EffRange::dump(ofstream& dumpf){
  dumpf << "Effective range: " << effrange <<" ";
}

void EffRange::restart(Istrm& restartf){
  restartf.validate("Effective range",58);
  restartf >> effrange;
}

float EffRange::get(){
  return effrange;
}
