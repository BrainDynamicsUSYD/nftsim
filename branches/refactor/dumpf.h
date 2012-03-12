/***************************************************************************
                          dumpf.h  -  dumps data into a file
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef DUMPF_H
#define DUMPF_H

#include<sstream>
using std::stringstream;
#include<iostream>
using std::istream;
#include<string>
using std::string;

#include<fstream>
using std::ofstream;
using std::ostream;
#include<string>
using std::string;

class Dumpf
{
  Dumpf(Dumpf&);
  Dumpf& operator=(Dumpf&);

  ofstream file;
  stringstream ss;
  ostream* s;
public:
  Dumpf(void);
  ~Dumpf(void);
  void open( const string& filename );
  void verbose(void);
  void silent(void);
  Dumpf& operator<< ( double f );
  Dumpf& operator<< ( const string& str );
  Dumpf& operator<< ( int i );
  Dumpf& operator<< ( unsigned int i );
  Dumpf& operator<< ( ostream& (*pf)(ostream&) ); // for manupulators eg endl
};

ostream& septor( ostream& os ); // implements the separator "|" between fields
ostream& dspace( ostream& os ); // implements the separator "  " between node
ostream& setw( ostream& os );

#endif
