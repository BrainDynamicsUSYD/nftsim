/***************************************************************************
                          istrm.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef ISTRM_H
#define ISTRM_H

#include<fstream>
using std::ifstream;
using std::ofstream;
using std::ios;
#include<iostream>
using std::cerr;
using std::endl;
#include<cstring>
using std::strlen;
using std::strcmp;

class Istrm: public ifstream {
public: 
  Istrm(const char* filename);
  ~Istrm();
  void validate(const char* check, char delim);
private:
  char * pbuffer;
};

#endif
