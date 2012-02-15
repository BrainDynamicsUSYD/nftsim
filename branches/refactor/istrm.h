/***************************************************************************
                          istrm.h  -  an extension of istream
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef ISTRM_H
#define ISTRM_H

#include<fstream>
#include<iostream>
#include<string>
using std::string;
#include<vector>
using std::vector;
#include<cstdlib>
using std::endl;

class Istrm: private std::ifstream {
public: 
  Istrm(const char* filename);
  ~Istrm();
  // Looks for the next parameter called "param" and stores it in T
  // If "param" is not found, terminate program
  template<class T> void Param(const string& param, T& ret, int delim=':' );
  // Looks for the next parameter called "param" and stores it in T
  // If "param" is not found, return false
  template<class T> bool Optional( const string& param, T& ret, int delim=':' );
  // Read & return an arbitrary array of doubles
  vector<double> Numbers(void);
  // Find the next "Check", then returns the next input entry as string
  string Find( const string& Check );
  bool Next( const string& Check, int delim=':' );
  // ignores up to delim
  Istrm& ignore( int delim=':' );
private:

  Istrm(const Istrm& other); // No copy constructor
  Istrm(); // No default constructor
  Istrm& operator=(const Istrm& other); // No assignment operator
  char* buffer; // character buffer with size 200 bytes
  uint filesize;
};

#endif
