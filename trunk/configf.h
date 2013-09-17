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
using std::cerr;
using std::endl;

class Configf : protected std::ifstream // derived class 
{
  Configf(const Configf& other); // No copy constructor
  Configf(); // No default constructor
  Configf& operator=(const Configf& other); // No assignment operator

  char* buffer;
  size_t filesize;
public: 
  Configf( const char* filename ); // const ptr to filename for ifstream
  virtual ~Configf(void);

  // Looks for the next parameter called "param" and stores it in T
  // If "param" is not found, terminate program
  template<class T> void param(const string& param, T& ret, int delim=':' );
  // Looks for the next parameter called "param" and stores it in T
  // If "param" is not found, return false
  template<class T> bool optional( const string& param, T& ret, int delim=':' );
  // Read & return an arbitrary array of doubles
  vector<double> numbers(void);
  // Return all whitespace separated strings before delimiting string
  vector<string> arb( const string& delim );
  // Find the next "Check", then returns the next input entry as string
  string find( const string& Check );

  // points to next delim, and verify it is "check"+"delim"
  bool next( const string& Check, int delim=':' );
  // searches and points to next keyword
  void go2( const string& keyword );

  int tell(void) { return std::ifstream::tellg(); }
  void seek( int position ) { std::ifstream::seekg(position); }
};

// global function that returns string=="Object#" for config file parsing
// also useful in naming outputf "solution.phi.#"
string label( const string& prefix, int index );

// Restartf does nothing new
class Restartf : public Configf // derived from Configf also a ifstream
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
      cerr << "Input variable '" << param << "' is given a wrong type."
        << endl;
      exit(EXIT_FAILURE);
    }
    *this >> ret;
  }
  else {
    int position = tellg();
    seekg(0,std::ios::beg);
    read(buffer,position);
    buffer[position] = '\0';
    cerr << "Unable to find next input variable: "
      << param << endl
      << "From configuration file:" << endl
      << buffer << endl;
    exit(EXIT_FAILURE);
  }
}

template<> inline void Configf::param<string>(const string& param, string& ret, int delim )
{
  if( next(param,delim) ) {
    *this >> buffer;
    if( !good() ) {
      cerr << "Input variable '" << param << "' is given a wrong type."
        << endl;
      exit(EXIT_FAILURE);
    }
    ret = string(buffer);
  }
  else {
    exit(EXIT_FAILURE);
  }
}

template<class T> bool Configf::optional( const string& param, T& ret, int delim )
{
  if( next(param,delim) ) {
    *this >> ret;
    if( !good() ) {
      cerr << "Input variable '" << param << "' is given a wrong type."
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
