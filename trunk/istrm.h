/***************************************************************************
                          istrm.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef ISTRM_H
#define ISTRM_H

#include<fstream>
#include<iostream>

class Istrm: public std::ifstream {
public: 
  Istrm(const char* filename);
  ~Istrm();
  int optional(const char* check, char delim);
  void validate(const char* check, char delim);
  double find(const char* check, char delim, int ordinal);
  int choose(const char* ch, char delim);
private:
  Istrm(Istrm& ); // no copy constructor
  char * pbuffer;
  Istrm(const Istrm& other); // Block copy constructor
  Istrm& operator=(const Istrm& other); // Block assignment operator
};

#endif
