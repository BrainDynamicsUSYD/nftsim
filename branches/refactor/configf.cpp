/***************************************************************************
                          configf.cpp  -  Extensions to input streams for parsing
                                        the input file
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cstdlib>
#include<string>
using std::string;
#include<vector>
using std::vector;
#include<list>
using std::list;
#include"configf.h"
#include"nf.h"
using std::endl;

template void Configf::Param
  <double>(const string& param, double& ret, int delim=':' );
template void Configf::Param
  <int>(const string& param, int& ret, int delim=':' );
template void Configf::Param
  <NF>(const string& param, NF& ret, int delim=':' );

template bool Configf::Optional
  <double>( const string& param, double& ret, int delim=':' );

Configf::Configf( const char* filename )
  : std::ifstream(filename)
{
  if( !*this ) {
    std::cerr << "Unable to open configuration file." << endl;
    exit(EXIT_FAILURE);
  }
  int sp = tellg(); // store current file position
  seekg(0,std::ios::end);
  filesize = tellg();
  buffer = new char[filesize];
  seekg(sp);
}

Configf::~Configf()
{
  delete[] buffer;
}

// Reads an arbitrary number of doubles and return it in an array
vector<double> Configf::Numbers(void)
{
  vector<double> ret;
  while( good() ) { // read until non-numeral
    double buffer; *this >> buffer;
	ret.push_back( buffer );
  }
  ret.pop_back(); // deletes last element==0
  clear(); // clear the fail bit
  return ret;
}

string Configf::Find( const string& Check )
{
/* 
This function implements a wildcard search that searches the config file assuming
a structure like
<unique>: extra_data - key1:value key2:value
The search term Check takes the form "<unique>*keyn" or "<unique>"
and this function returns a string representation of the value associated 
with the key that was being searched for. If the search term is "<unique>"
then a string representation of extra_data is returned. The file pointer is 
returned to the same position as it was originally. Search is CASE SENSITIVE
*/

  if( Check.empty() ) {
    std::cerr << "Attempted to use Configf::Find searching for an empty string" << endl;
    exit(EXIT_FAILURE);
  }
  string result;
  int sp = tellg(); // store current file position
  seekg(0,std::ios::beg);
  read(buffer,filesize);
  string file_content(buffer);
  size_t wildcard_pos = Check.find('*');
  size_t match = file_content.find(Check.substr(0,wildcard_pos));
  if( match == string::npos) {
    std::cerr << "Failed to find first part of string" << endl;
    exit(EXIT_FAILURE);
  }
  
  // skip searching for the second part if a star is not present
  if(wildcard_pos != string::npos){
      match = file_content.find(Check.substr(wildcard_pos+1,Check.length()),match);
      if( match == string::npos) {
        std::cerr << "Failed to find second part of string" << endl;
        exit(EXIT_FAILURE);
      }
    match += Check.substr(wildcard_pos+1,Check.length()).length();

  }
  else{
    match += Check.length();
  }
  
  // adjust the position if the user didn't include a colon in the search term
  if(Check[Check.length()-1] != ':')
    match += 1;
  seekg(0+match);
  *this >> result;
  seekg(sp);
  return string(result);
}

bool Configf::Next( const string& Check, int delim )
{
  if( Check.empty() ) {
    std::cerr << "Configf validating an empty string." << endl;
    exit(EXIT_FAILURE);
  }
  std::streampos sp = tellg();
  getline( buffer, filesize, delim );
  if( !good() ) {
    std::cerr << "NeuroField cannot read configuration file." << endl;
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

Restartf::Restartf( const char* filename )
    : Configf(filename)
{
}

Restartf::~Restartf(void)
{
}

string label( const string& prefix, int index )
{
  stringstream ss;
  ss<<prefix.c_str()<<index;
  return ss.str();
}
