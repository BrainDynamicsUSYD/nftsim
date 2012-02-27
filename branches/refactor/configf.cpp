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
  if( Check.empty() ) {
    std::cerr << "Configf validating an empty string." << endl;
    exit(EXIT_FAILURE);
  }

  // implements wildcard '*' by dividing Check into substrings
  list<uint> wild; // stores the location of wildcard
  wild.push_back(0);
  string NoStar(Check); // eliminate the stars from Check
  list<string> match; // substrings of Check
  for( uint i=0; i<Check.length(); i++ )
    if( Check[i] == '*' ) {
      wild.push_back(i);
      NoStar.erase(i,1);
    }
  wild.push_back( NoStar.length() );

  match.push_back( Check.substr( *wild.begin(), *++wild.begin() ) );
  for( list<uint>::iterator it = wild.begin(); it != --wild.end(); )
    match.push_back( NoStar.substr( *it, *++it ) );

  int sp = tellg(); // store current file position
  seekg(0,std::ios::beg);
  read(buffer,filesize);
  string file_content(buffer);
  string sub;
  uint current = 0; // the current'th match
  for( list<string>::iterator it = match.begin(); it != match.end(); it++ )
    if( ( current = file_content.find( *it, current ) )!= std::string::npos )
      if( it==--match.end() ) {
        seekg(current+Check.length());
        char temp[filesize]; *this >> temp;
        seekg(sp);
        return string(temp);
      }
      else
        continue;
    else {
      std::cerr << "Unable to find essential object:'"
        << Check << "'." << endl;
      exit(EXIT_FAILURE);
    }
    std::cerr << "Unable to find essential object:'"
      << Check << "'." << endl;
    exit(EXIT_FAILURE);
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

/*Restartf::Restartf( const char* filename )
    : Configf(filename)
{
}

Restartf::~Restartf(void)
{
}*/

string label( const string& prefix, int index )
{
  stringstream ss;
  ss<<prefix.c_str()<<index;
  return ss.str();
}
