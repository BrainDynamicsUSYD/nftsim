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

void Gamma::init(ifstream& inputf){
  inputf.ignore(200,58); // throwaway everything uptil colon character
  inputf >> gamma;
}

void Gamma::dump(ofstream& dumpf){
  dumpf << "gamma :" << gamma << " ";
}

void Gamma::restart(ifstream& restartf){
  restartf.ignore(200,58); // throwaway everything uptil colon character
  restartf >> gamma;
}

float Gamma::get(){
  return gamma;
}
