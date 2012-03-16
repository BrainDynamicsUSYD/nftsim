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
}

Output::~Output(void)
{
}

void Output::step(void)
{
  for( uint i=0; i<node.size(); i++ )
    dumpf<<space<<field[node[i]]; // nodes separated by double space
  dumpf<<space<<space<<septor; // delimit fields via '|'
}

const string& Output::fieldname(void) const
{
  return name;
}
