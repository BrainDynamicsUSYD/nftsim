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

void Parameter::init(double initval){
  param = initval;
}

void Parameter::dump(ofstream& dumpf){
  dumpf << ident << ":" << param << " ";
}

double Parameter::get(){
  return param;
}
