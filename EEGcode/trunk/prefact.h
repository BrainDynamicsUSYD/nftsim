/***************************************************************************
                          prefact.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PREFACT_H
#define PREFACT_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"parameter.h"

class Prefact {
public: 
  Prefact(long gsize,long longside,long shortside);
  ~Prefact();
  void precalcfact(double* kvect, double deltax, long centrex, long centrey);
  double* factRe;
  double* factIm;
private:
  void wrapbndry(double* array);
  long gridsize;
  long longsidelength;
  long shortsidelength;
};

#endif
