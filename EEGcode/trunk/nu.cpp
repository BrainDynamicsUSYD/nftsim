/***************************************************************************
                          nu.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "nu.h"

Nu::Nu(){
}
Nu::~Nu(){
}

void Nu::init(ifstream& inputf){
  inputf.ignore(200,58); // throwaway every before the colon character
  inputf >> nu;
}

void Nu::dump(ofstream& dumpf){
  dumpf << "Nu: " << nu << " ";
}

void Nu::restart(ifstream& restartf){
  restartf.ignore(200,58); // throwaway every before the colon character
  restartf >> nu;
}

float Nu::get(){
  return nu;
}

