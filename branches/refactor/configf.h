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
  template<class T> void param(const string& param, T& ret, int delim=':' );
  // Looks for the next parameter called "param" and stores it in T
  // If "param" is not found, return false
  template<class T> bool optional( const string& param, T& ret, int delim=':' );
  // Read & return an arbitrary array of doubles
  vector<double> numbers(void);
  // Find the next "Check", then returns the next input entry as string
  string find( const string& Check );

  // points to next delim, and verify it is "check"+"delim"
  bool next( const string& Check, int delim=':' );
  // searches and points to next keyword
  void go2( const string& keyword );
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

template<class T> void Configf::param(const string& param, T& ret, int delim )
{
  if( next(param,delim) ) {
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

template<> inline void Configf::param<string>(const string& param, string& ret, int delim )
{
  if( next(param,delim) ) {
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

template<class T> bool Configf::optional( const string& param, T& ret, int delim )
{
  if( next(param,delim) ) {
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
