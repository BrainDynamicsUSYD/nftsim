#include<iomanip>
#include<cstdlib>
using std::setw;
#include<iostream>
using std::endl;
#include"dumpf.h"

Dumpf::Dumpf(void)
{
}

Dumpf::~Dumpf(void)
{
  if( file.is_open() )
    file.close();
}

void Dumpf::open( const string& filename )
{
  file.open( filename.c_str() );
  if( !file ) {
    std::cerr<<"Error, cannot open file'"<<filename.c_str()<<"' for output."
        <<endl;
    exit(EXIT_FAILURE);
  }
  file.precision(14); file<<std::scientific;
}

Dumpf& Dumpf::operator<< ( double f )
{
  file<<f;
  return *this;
}

Dumpf& Dumpf::operator<< ( const string& s )
{
  file<<setw(20)<<s.c_str();
  return *this;
}

Dumpf& Dumpf::operator<< ( int i )
{
  file<<i;
  return *this;
}

Dumpf& Dumpf::operator<< ( unsigned int i )
{
  file<<i;
  return *this;
}

Dumpf& Dumpf::operator<< ( ostream& (*pf)(ostream&) )
{
  file<<pf;
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
