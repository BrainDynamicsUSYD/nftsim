/***************************************************************************
                          istrm.cpp  -  Extensions to input streams for parsing
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

#include"istrm.h"
using std::endl;

Istrm::Istrm(const char* filename)
  : std::ifstream(filename,std::ios::in)
{
  /*int sp = tellg(); // store current file position
  seekg(0,std::ios::end);
  int filesize = tellg();
  buffer = new char[filesize];
  seekg(sp);*/
}

Istrm::~Istrm()
{
  delete[] buffer;
}

// Reads an arbitrary number of doubles and return it in an array
vector<double> Istrm::Numbers(void)
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

string Istrm::Find( const string& Check )
{
  if( Check.empty() ) {
    std::cerr << "Istrm validating an empty string." << endl;
    exit(EXIT_FAILURE);
  }

  // implements wildcard '*' by dividing Check into substrings
  list<uint> wild; // stores the location of wildcard
  uint current = 0; wild.push_back(0);
  do {
    wild.push_back(current);
  } while( std::string::npos !=( current = Check.find("*",wild.back()+1) ) );
  list<string> match;
  for( list<uint>::iterator it = wild.begin(); it != wild.end(); )
    match.push_back( Check.substr( *it, *++it ) );

  int sp = tellg(); // store current file position
  read(buffer,sizeof(buffer));
  string file_content(buffer);
  string sub;
  current = 0; // the current'th match
  for( list<string>::iterator it = match.begin(); it != match.end(); it++ )
    if( ( current = file_content.find( *it, current ) )!= std::string::npos )
      if( it==match.end() ) {
        *this >> buffer;
        seekg(sp);
        return string(buffer);
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
