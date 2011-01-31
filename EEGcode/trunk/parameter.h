/***************************************************************************
                          parameter.h  -  description
                             -------------------
    copyright            : (C) 2006 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PARAMETER_H
#define PARAMETER_H

#include<fstream>
#include<iostream>
#include<string>

#include"istrm.h"

class Parameter {
public: 
  Parameter(const char *,Istrm& inputf);
  Parameter(const char *,double initval);
  ~Parameter();
  double get(){return param;};
  void dump(std::ofstream& dumpf);
private:
  Parameter(Parameter& ); // no copy constructor
  double param;
  std::string ident;
};

#endif
