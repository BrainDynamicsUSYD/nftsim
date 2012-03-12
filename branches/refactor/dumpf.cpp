#include<iomanip>
#include<cstdlib>
using std::setw;
#include<iostream>
using std::endl;
#include"dumpf.h"

Dumpf::Dumpf(void)
{
  ss.precision(14); ss<<std::scientific;
  silent(); // defaults to output when program terminates
}

Dumpf::~Dumpf(void)
{
  if( s == &ss )
    if( file.is_open() ) {
      file << ss.str().c_str();
      file.close();
    }
}

void Dumpf::open( const string& filename )
{
  if( file.is_open() ) {
    std::cerr<<"Attempting to dump into two files."<<endl;
    file.close();
    exit(EXIT_FAILURE);
  }
  file.open( filename.c_str() );
  if( !file ) {
    std::cerr<<"Error, cannot open file'"<<filename.c_str()<<"' for output."
        <<endl;
    exit(EXIT_FAILURE);
  }
}

void Dumpf::verbose(void)
{
  s = &file;
}

void Dumpf::silent(void)
{
  s = &ss;
}

Dumpf& Dumpf::operator<< ( double f )
{
  *s<<f;
  return *this;
}

Dumpf& Dumpf::operator<< ( const string& str )
{
  *s<<setw(20)<<str.c_str();
  return *this;
}

Dumpf& Dumpf::operator<< ( int i )
{
  *s<<i;
  return *this;
}

Dumpf& Dumpf::operator<< ( unsigned int i )
{
  *s<<i;
  return *this;
}

Dumpf& Dumpf::operator<< ( ostream& (*pf)(ostream&) )
{
  *s<<pf;
  return *this;
}

ostream& septor( ostream& os )
{
  os<<"|";
  return os;
}

ostream& dspace( ostream& os )
{
  os<<"  ";
  return os;
}

ostream& setw( ostream& os )
{
  os<<setw(20);
  return os;
}
