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
 
void Alpha::init(ifstream& inputf){
  inputf.ignore(200,58); //throwawy every before colon character
  inputf >> alpha;
}

void Alpha::dump(ofstream& dumpf){
  dumpf << "alpha :" << alpha << " ";
}

void Alpha::restart(ifstream& restartf){
  restartf.ignore(200,58); //throwawy every before colon character
  restartf >> alpha;
}

float Alpha::get(){
  return alpha;
}
