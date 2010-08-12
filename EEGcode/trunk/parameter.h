/***************************************************************************
                          parameter.h  -  description
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PARAMETER_H
#define PARAMETER_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;
#include<string>
using std::string;

#include"istrm.h"

class Parameter {
public: 
  Parameter(const char *,Istrm& inputf);
  Parameter(const char *,double initval);
  ~Parameter();
  double get(){return param;};
  void dump(ofstream& dumpf);
private:
  Parameter(Parameter& ); // no copy constructor
  double param;
  string ident;
};

#endif
