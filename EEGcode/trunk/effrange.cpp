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

void EffRange::init(ifstream& inputf){
  inputf.ignore(200,58); // throwaway every before the colon character
  inputf >> effrange;
}

void EffRange::dump(ofstream& dumpf){
  dumpf << "Effective range: " << effrange <<" ";
}

void EffRange::restart(ifstream& restartf){
  restartf.ignore(200,58); // throwaway every before the colon character
  restartf >> effrange;
}

float EffRange::get(){
  return effrange;
}
