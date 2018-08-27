/** @file configf.h
  @brief Declaration of the class used to read information from `.conf` files.

  The Configf class is derived from std::ifstream, it provides additional member
  functions to help parse NFTsim's `.conf` files.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_CONFIGF_H
#define NFTSIM_SRC_CONFIGF_H

// C++ standard library headers
#include <fstream>  // std::ifstream; tellg; seekg;
#include <iostream> // std::endl; std::cerr; std::streamsize; std::ios::beg;
#include <string>   // std::string;
#include <vector>   // std::vector;


class Configf : public std::ifstream { // derived class
  char* buffer;
  std::streamsize filesize;
 public:
  Configf(const Configf&) = delete;  ///< No copy constructor allowed.
  Configf() = delete;                ///< No default constructor allowed.
  Configf& operator=(const Configf&) = delete; ///< No copy assignment operator allowed.


  explicit Configf( const std::string filename ); // const string of filename for ifstream
  ~Configf() override;

  /// Looks for the next parameter called "param" and stores it in T.
  /** If "param" is not found, terminate program. */
  template<class T> void param(const std::string& param, T& ret, int delim=':' );

  /// Looks for the next parameter called "param" and stores it in T
  /** If "param" is not found, return false. */
  template<class T> bool optional( const std::string& param, T& ret, int delim=':' );

  /// Read & return an arbitrary array of doubles
  std::vector<double> numbers();

  /// Return all whitespace separated strings before delimiting string
  std::vector<std::string> arb( const std::string& delim );

  /// Find the next "Check", then returns the next input entry as string
  std::string find( const std::string& Check );

  /// Points to next delim, and verify it is "check"+"delim"
  bool next( const std::string& Check, int delim=':' );

  /// Searches and points to next keyword
  void go2( const std::string& keyword );

  std::streamsize tell() {
    return std::ifstream::tellg();
  }
  void seek( std::streamsize position ) {
    std::ifstream::seekg(position);
  }

  friend class Dumpf;
};

/// global function that returns string=="Object#" for config file parsing
/** also useful in naming outputf "solution.phi.#". */
std::string label( const std::string& prefix, size_t index );

template<class T> void Configf::param(const std::string& param, T& ret, int delim ) {
  if( next(param,delim) ) {
    if( !good() ) {
      std::cerr << "Input variable '" << param << "' is given a wrong type."
           << std::endl;
      exit(EXIT_FAILURE);
    }
    *this >> ret;
  } else {
    std::streamsize position = tellg();
    seekg(0,std::ios::beg);
    read(buffer,position);
    buffer[position] = '\0';
    std::cerr << "Unable to find next input variable: "
         << param << std::endl
         << "From configuration file:" << std::endl
         << buffer << std::endl;
    exit(EXIT_FAILURE);
  }
}

template<> inline void Configf::param<std::string>(const std::string& param,
                                                   std::string& ret, int delim ) {
  if( next(param,delim) ) {
    *this >> buffer;
    if( !good() ) {
      std::cerr << "Input variable '" << param << "' is given a wrong type."
           << std::endl;
      exit(EXIT_FAILURE);
    }
    ret = std::string(buffer);
  } else {
    exit(EXIT_FAILURE);
  }
}

template<class T> bool Configf::optional( const std::string& param, T& ret, int delim ) {
  if( next(param,delim) ) {
    *this >> ret;
    if( !good() ) {
      std::cerr << "Input variable '" << param << "' is given a wrong type."
           << std::endl;
      exit(EXIT_FAILURE);
    }
    return true;
  }
  return false;
}

#endif //NFTSIM_SRC_CONFIGF_H
