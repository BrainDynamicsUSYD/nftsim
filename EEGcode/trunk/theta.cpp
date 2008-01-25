/***************************************************************************
                          theta.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "theta.h"

Theta::Theta(){
}
Theta::~Theta(){
}

void Theta::init(ifstream& inputf){
  inputf.ignore(200,58); // throwaway uptil colon
  inputf >> theta;
}

void Theta::dump(ofstream& dumpf){
  dumpf << "Theta:" << theta <<" ";
}

void Theta::restart(ifstream & restartf){
  restartf.ignore(200,58); // throwaway uptil colon
  restartf >> theta;
}

float Theta::get(){
  return theta;
}
