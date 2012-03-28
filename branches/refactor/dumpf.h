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
  string filename;
  void open(void);
  void checkFlush(void);
public:
  Dumpf(void);
  ~Dumpf(void);
  void open( const string& filename );
  void verbose(void);
  Dumpf& operator<< ( double f );
  Dumpf& operator<< ( const string& str );
  Dumpf& operator<< ( int i );
  Dumpf& operator<< ( ostream& (*pf)(ostream&) ); // for manupulators eg endl
};

ostream& septor( ostream& os ); // implements the separator "|" between fields
ostream& space( ostream& os ); //  puts a space into dumpfile
ostream& setw( ostream& os );

#endif
