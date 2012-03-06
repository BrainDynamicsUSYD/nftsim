/***************************************************************************
                          output.cpp  -  outputs a field
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
#include"output.h"
using std::endl;

vector<int> Output::node; // vector of nodes to output
Dumpf Output::dumpf; // file to dump

void Output::init( Configf& configf )
{
}

void Output::restart( Restartf& restartf )
{
}

void Output::dump( Dumpf& dumpf ) const
{
}

Output::Output( const string& name, const vector<double>& field )
    : NF(0,0,0), name(name), field(field)
{
  /*if( !stream ) {
    std::cerr<<"Error, cannot open file '"<<filename.c_str()<<"' for output."
        <<endl;
    exit(EXIT_FAILURE);
  }
  stream.precision(14);stream<<std::scientific;*/
}

Output::~Output(void)
{
  /*if( stream.is_open() )
    stream.close();*/
}

void Output::step(void)
{
  dumpf<<"  |"; // delimit fields via '|'
  for( uint i=0; i<node.size(); i++ )
    dumpf<<"  "<<field[node[i]]; // nodes separated by double space
}

const string& Output::fieldname(void) const
{
  return name;
}
