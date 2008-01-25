/***************************************************************************
                          parameter.cpp  -  description
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "parameter.h"

Parameter::Parameter(const char * pname):ident(pname){
}
Parameter::~Parameter(){
}
 
void Parameter::init(Istrm& inputf){
  inputf.validate(ident.c_str(),58);
  inputf >> param;
}

void Parameter::dump(ofstream& dumpf){
  dumpf << ident << ":" << param << " ";
}

void Parameter::restart(Istrm& restartf){
  restartf.validate(ident.c_str(),58);
  restartf >> param;
}

float Parameter::get(){
  return param;
}
