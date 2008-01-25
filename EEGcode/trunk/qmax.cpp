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

void Qmax::init(Istrm& inputf){
  inputf.validate("Qmax",58);
  inputf >> qmax;
}

void Qmax::dump(ofstream& dumpf){
  dumpf << "Qmax :" << qmax << " ";
}

void Qmax::restart(Istrm& restartf){
  restartf.validate("Qmax",58);
  restartf >> qmax;
}

float Qmax::get(){
  return qmax;
}
