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

void Nu::init(Istrm& inputf){
  inputf.validate("Nu",58);
  inputf >> nu;
}

void Nu::dump(ofstream& dumpf){
  dumpf << "Nu: " << nu << " ";
}

void Nu::restart(Istrm& restartf){
  restartf.validate("Nu",58);
  restartf >> nu;
}

float Nu::get(){
  return nu;
}

