/***************************************************************************
                          field.h  -  Store a field of data for 
                              propagator use with rectangular boundaries
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef FIELD_H
#define FIELD_H

#include<fstream>
#include<iostream>
#include<string>
#include"configf.h"

class Field {
public: 
  Field(long gridsize,long longsidelength,long shortsidelength,const char * fname);
  ~Field();
  void init(Configf& inputf);
  void init(double Uinit);
  void init(double* Uinit);
  void dump(std::ofstream& dumpf);
  void restart(Configf& restartf);
  void update(double* U);
  double *U_1; // Pointer to Eta one step in the past for field variable
  double *U_2; // Pointer to Eta two steps in the past for field variable  
private:
  Field(Field& ); // no copy constructor
  std::string ident; // Name of field type
  void copyarr(double* Ufrom, double* Uto);
  double *U_A; // One of two keys in keyring storing field one and two steps in past
  double *U_B; // One of two keys in keyring storing field one and two steps in past  
  const long gridsize; // size of grid for integration of wave equation
  const long longsidelength;
  const long shortsidelength;
  long startfirstrow;
 };

#endif
