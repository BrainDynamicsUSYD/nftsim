/** @file configf.cpp
  @brief Definition  of the class used to read information from `.conf` files.

  The Configf class is derived from std::ifstream, it provides additional member
  functions to help parse NFTsim's `.conf` files.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "configf.h"    // Configf;

// C++ standard library headers
#include <iostream> // std::streamoff; std::ios::beg; std::ios::end; std::streampos; std::ifstream;
#include <sstream>  // std::stringstream;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

Configf::Configf( const string filename )
  : std::ifstream(filename) {
  if( !*this ) {
    cerr << "Unable to open configuration file." << endl;
    exit(EXIT_FAILURE);
  }
  std::streamoff sp = tellg(); ///< stores current file position
  seekg(0,std::ios::end);
  filesize = tellg();
  buffer = new char[filesize+1];
  buffer[filesize] = '\0';
  seekg(sp);
}

Configf::~Configf() {
  delete[] buffer;
}

/// Reads an arbitrary number of doubles and return it in an array
vector<double> Configf::numbers() {
  vector<double> ret;
  while( good() ) { // read until non-numeral
    double buffer;
    *this >> buffer;
    ret.push_back( buffer );
  }
  ret.pop_back(); // deletes last element==0
  clear(); // clear the fail bit
  return ret;
}

vector<string> Configf::arb( const string& delim ) {
  string temp;
  vector<string> ret_value;
  while( temp!=delim && *this ) {
    *this >> temp;
    ret_value.push_back(temp);
    if( bad() ) {
      cerr<<"Error reading configuration file while reading arbitrary strings."
          <<endl;
      exit(EXIT_FAILURE);
    }
  }
  ret_value.pop_back();
  for( size_t i=0; i<delim.size(); i++ ) {
    unget();
  }
  return ret_value;
}

/**
  This function implements a wildcard search that searches the config file
  assuming a structure like:

      <unique>: extra_data - key1:value key2:value

  The search term Check takes the form "<unique>*keyn" or "<unique>"
  and this function returns a string representation of the value associated
  with the key that was being searched for. If the search term is "<unique>"
  then a string representation of extra_data is returned. The file pointer is
  returned to the same position as it was originally.
  Search is CASE SENSITIVE
*/
string Configf::find( const string& Check ) {

  if( Check.empty() ) {
    cerr << "Attempted to use Configf::Find searching for an empty string"
         << endl;
    exit(EXIT_FAILURE);
  }
  string result;
  std::streamoff sp = tellg(); // store current file position
  seekg(0,std::ios::beg);
  read(buffer,filesize);
  string file_content(buffer);
  size_t wildcard_pos = Check.find('*');
  size_t match = file_content.find(Check.substr(0,wildcard_pos));
  if( match == string::npos) {
    cerr << "Failed to find '" << Check.substr(0,wildcard_pos) << "' which was the first part of the search term '" << Check << "'" << endl;
    exit(EXIT_FAILURE);
  }

  // skip searching for the second part if a star is not present
  if(wildcard_pos != string::npos) {
    match = file_content.find(Check.substr(wildcard_pos+1,Check.length()),match);
    if( match == string::npos) {
      seekg(sp);
      return string("");
      /*cerr << "Failed to find '" << Check.substr(wildcard_pos+1,Check.length()) << "' which was the second part of the search term '" << Check << "'" << endl;
          exit(EXIT_FAILURE);*/
    }
    match += Check.substr(wildcard_pos+1,Check.length()).length();

  } else {
    match += Check.length();
  }

  // adjust the position if the user didn't include a colon in the search term
  if(Check[Check.length()-1] != ':') {
    match += 1;
  }
  seekg(0+match);
  *this >> result;
  seekg(sp);
  return string(result);
}

bool Configf::next( const string& Check, int delim ) {
  if( Check.empty() ) {
    cerr << "Configf validating an empty string." << endl;
    exit(EXIT_FAILURE);
  }
  std::streampos sp = tellg();
  getline( buffer, filesize, delim );
  if( !good() ) {
    cerr << "Error reading config file when looking for: " << Check << endl;
    exit(EXIT_FAILURE);
  }
  string param(buffer);
  int pos = param.find( Check, param.length()-Check.length() );
  if( pos == -1 ) {
    seekg(sp);
    return false;
  }
  return true;
}

void Configf::go2( const string& keyword ) {
  seekg(0,std::ios::beg);
  if( keyword.empty() ) {
    cerr << "Searching for an empty string in configuration file." << endl;
    exit(EXIT_FAILURE);
  }
  read(buffer,filesize);
  string file_content(buffer);
  if( file_content.find(keyword) == string::npos ) {
    cerr<<"Configuration reading error: keyword "<<keyword<<"not found."
        <<endl;
    exit(EXIT_FAILURE);
  }
  seekg( file_content.find(keyword)-1 );
}

string label( const string& prefix, size_t index ) {
  stringstream ss;
  ss<<prefix.c_str()<<index;
  return ss.str();
}
