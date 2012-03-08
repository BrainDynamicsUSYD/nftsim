#include<iomanip>
#include<cstdlib>
using std::setw;
#include<iostream>
using std::endl;
#include"dumpf.h"

Dumpf::Dumpf(void)
{
  ss.precision(14); ss<<std::scientific;
}

Dumpf::~Dumpf(void)
{
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

Dumpf& Dumpf::operator<< ( double f )
{
  ss<<f;
  return *this;
}

Dumpf& Dumpf::operator<< ( const string& s )
{
  ss<<setw(20)<<s.c_str();
  return *this;
}

Dumpf& Dumpf::operator<< ( int i )
{
  ss<<i;
  return *this;
}

Dumpf& Dumpf::operator<< ( unsigned int i )
{
  ss<<i;
  return *this;
}

Dumpf& Dumpf::operator<< ( ostream& (*pf)(ostream&) )
{
  ss<<pf;
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
