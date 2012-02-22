/***************************************************************************
                          outputf.cpp  -  outputs a field into an ofstream
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
#include"outputf.h"
using std::endl;

void Outputf::init( Configf& configf )
{
}

void Outputf::restart( Restartf& restartf )
{
}

void Outputf::dump( Dumpf& dumpf ) const
{
}

Outputf::Outputf( int nodes, double deltat, int index, const vector<int>& node,
    const string& filename, const vector<double>& field )
    : NF(nodes,deltat,index), node(node), stream(filename.c_str()), field(field)
{
  if( !stream ) {
    std::cerr<<"Error, cannot open file '"<<filename.c_str()<<"' for output."
        <<endl;
    exit(EXIT_FAILURE);
  }
  stream.precision(14);
}

Outputf::~Outputf(void)
{
  if( stream.is_open() )
    stream.close();
}

void Outputf::step(void)
{
  for( uint i=0; i<node.size(); i++ )
    stream<<field[node[i]]<<" ";
  stream<<endl;
}
