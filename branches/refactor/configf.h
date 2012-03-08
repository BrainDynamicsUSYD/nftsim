/***************************************************************************
                          configf.h  -  an extension of istream
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef CONFIGF_H
#define CONFIGF_H

#include<sstream>
using std::stringstream;
#include<string>
using std::string;

#include<fstream>
#include<iostream>
#include<string>
using std::string;
#include<vector>
using std::vector;
#include<cstdlib>
using std::endl;

class Configf : protected std::ifstream
{
  Configf(const Configf& other); // No copy constructor
  Configf(); // No default constructor
  Configf& operator=(const Configf& other); // No assignment operator

  char* buffer;
  uint filesize;
public: 
  Configf( const char* filename );
  virtual ~Configf(void);
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
};

// global function that returns string=="Object#" for config file parsing
// also useful in naming outputf "solution.phi.#"
string label( const string& prefix, int index );

// Restartf does nothing new
class Restartf : public Configf
{
  Restartf(const Restartf&);
  Restartf();
  Restartf& operator=(Restartf&);
public:
  Restartf( const char* filename );
  virtual ~Restartf(void);
};
//class Restartf;

template<class T> void Configf::Param(const string& param, T& ret, int delim )
{
  if( Next(param,delim) ) {
    if( !good() ) {
      std::cerr << "Input variable '" << param << "' is given a wrong type."
        << endl;
      exit(EXIT_FAILURE);
    }
    *this >> ret;
  }
  else {
  std::cerr << "Unable to find next input variable :'"
    << param << "' "<< endl
    << "Last read was :'"
    << buffer
    << "' "<< endl
    << "Please check the sequence of parameters." << endl;
  exit(EXIT_FAILURE);
  }
}

template<> inline void Configf::Param<string>(const string& param, string& ret, int delim )
{
  if( Next(param,delim) ) {
    *this >> buffer;
    if( !good() ) {
      std::cerr << "Input variable '" << param << "' is given a wrong type."
        << endl;
      exit(EXIT_FAILURE);
    }
    ret = string(buffer);
  }
  else {
  std::cerr << "Unable to find next input variable :'"
    << param << "' "<< endl
    << "Last read was :'"
    << buffer
    << "' "<< endl
    << "Please check the sequence of parameters." << endl;
  exit(EXIT_FAILURE);
  }
}

template<class T> bool Configf::Optional( const string& param, T& ret, int delim )
{
  if( Next(param,delim) ) {
    *this >> ret;
    if( !good() ) {
      std::cerr << "Input variable '" << param << "' is given a wrong type."
        << endl;
      exit(EXIT_FAILURE);
    }
    return true;
  }
  else {
    return false;
  }
}

#endif
