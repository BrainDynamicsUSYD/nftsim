/***************************************************************************
                          Qmax.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "qmax.h"

Qmax::Qmax(){
}
Qmax::~Qmax(){
}

void Qmax::init(ifstream& inputf){
  inputf.ignore(200,58); // throwaway everything uptil colon
  inputf >> qmax;
}

void Qmax::dump(ofstream& dumpf){
  dumpf << "Qmax :" << qmax << " ";
}

void Qmax::restart(ifstream& restartf){
  restartf.ignore(200,58); // throwaway everything uptil colon
  restartf >> qmax;
}

float Qmax::get(){
  return qmax;
}
