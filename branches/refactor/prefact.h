/***************************************************************************
                          prefact.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PREFACT_H
#define PREFACT_H

#include"configf.h"

class Prefact {
public: 
  Prefact(long gsize,long longside,long shortside);
  ~Prefact();
  void precalcfact(double* kvect, double deltax, long centrex, long centrey);
  double* factRe;
  double* factIm;
private:
  Prefact(Prefact& ); //no copy constructor
  void wrapbndry(double* array);
  long gridsize;
  long longsidelength;
  long shortsidelength;
};

#endif
