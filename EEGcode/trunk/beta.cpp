/***************************************************************************
                          beta.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "beta.h"

Beta::Beta(){
}
Beta::~Beta(){
}

void Beta::init(Istrm& inputf){
  inputf.validate("beta",58);
  inputf >> beta;
}

void Beta::dump(ofstream& dumpf){
  dumpf << "beta: " << beta << endl;
}

void Beta::restart(Istrm& restartf){
  restartf.validate("beta",58); 
  restartf >> beta;
}

float Beta::get(){
  return beta;
}
