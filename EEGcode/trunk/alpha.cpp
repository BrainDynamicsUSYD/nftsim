/***************************************************************************
                          alpha.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "alpha.h"

Alpha::Alpha(){
}
Alpha::~Alpha(){
}
 
void Alpha::init(Istrm& inputf){
  inputf.validate("alpha",58);
  inputf >> alpha;
}

void Alpha::dump(ofstream& dumpf){
  dumpf << "alpha:" << alpha << " ";
}

void Alpha::restart(Istrm& restartf){
  restartf.validate("alpha",58);
  restartf >> alpha;
}

float Alpha::get(){
  return alpha;
}
