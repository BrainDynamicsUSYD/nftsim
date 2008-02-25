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
using std::cerr;
using std::endl;
#include<string>
using std::string;

#include"istrm.h"

class Parameter {
public: 
  Parameter(const char *);
  ~Parameter();
  double get();
  void init(Istrm& inputf);
  void init(double initval);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void restart(double initval);
private:
  double param;
  string ident;
};

#endif
