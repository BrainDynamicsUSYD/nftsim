/***************************************************************************
                          parameter.cpp  -  description
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "parameter.h"

Parameter::Parameter(const char * pname, Istrm& inputf):ident(pname){
  inputf.validate(ident.c_str(),58);
  inputf >> param;
}

Parameter::Parameter(const char * pname, double initval):ident(pname){
  param = initval;
}

Parameter::~Parameter(){
}
 
void Parameter::dump(ofstream& dumpf){
  dumpf << ident << ":" << param << " ";
}
