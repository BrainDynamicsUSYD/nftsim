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
  template<class T> void Param(const string& param, T& ret ) {
    if( Next(param) )
      *this >> ret;
    else {
    std::cerr << "Unable to find next input variable :'"
      << param << "' "<< endl
      << "Last read was :'"
      << buffer
      << "' "<< endl
      << "Please check the sequence of parameters" << endl;
    exit(EXIT_FAILURE);
    }
  }

  // Looks for the next parameter called "param" and stores it in T
  // If "param" is not found, return false
  template<class T> bool Optional( const string& param, T& ret ) {
    std::streampos sp = tellg();
    if( Next(param) ) {
      *this >> ret;
      return true;
    }
    else {
      seekg(sp);
      return false;
    }
  }
  // Read & return an arbitrary array of doubles
  vector<double> Numbers(void);

  // Find the next "Check", then returns the next input entry as string
  string Find( const string& Check );

  // ignores up to delim
  Istrm& ignore( int n=1, int delim=EOF ) {
    std::ifstream::ignore(n,delim);
    return *this;
  }

  using std::ifstream::operator!;
private:
  bool Next( const string& Check ) {
    if( Check.empty() ) {
      std::cerr << "Istrm validating an empty string." << endl;
      exit(EXIT_FAILURE);
    }
    getline(buffer, 200, ':' );
    if( !good() ) {
      std::cerr << "NeuroField cannot read configuration file." << endl;
      exit(EXIT_FAILURE);
    }
    string param(buffer);
    int pos = param.find_last_of(Check);
    if( pos == -1 || unsigned(pos) != param.length()-Check.length() )
      return false;
    return true;
  }
  Istrm(const Istrm& other); // No copy constructor
  Istrm(); // No default constructor
  Istrm& operator=(const Istrm& other); // No assignment operator
  char* buffer; // character buffer with size 200 bytes
};

#endif
