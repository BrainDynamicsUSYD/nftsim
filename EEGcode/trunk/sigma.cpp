/***************************************************************************
                          sigma.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "sigma.h"

Sigma::Sigma(){
}
Sigma::~Sigma(){
}

void Sigma::init(ifstream& inputf){
  inputf.ignore(200,58); // throwaway everything uptil colon
  inputf >> sigma;
}

void Sigma::dump(ofstream& dumpf){
  dumpf << "Sigma:" << sigma <<" ";
}

void Sigma::restart(ifstream& restartf){
  restartf.ignore(200,58); // throwaway everything uptil colon
  restartf >> sigma;
}

float Sigma::get(){
  return sigma;
}
