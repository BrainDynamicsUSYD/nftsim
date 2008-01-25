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

void Beta::init(ifstream& inputf){
  inputf.ignore(200,58); //throwaway everything before colon character
  inputf >> beta;
}

void Beta::dump(ofstream& dumpf){
  dumpf << "beta: " << beta << endl;
}

void Beta::restart(ifstream& restartf){
  restartf.ignore(200,58); //throwaway everything before colon character
  restartf >> beta;
}

float Beta::get(){
  return beta;
}
