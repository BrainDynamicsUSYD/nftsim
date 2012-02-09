/***************************************************************************
                          ostrm.cpp  -  outputs traces
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<fstream>
#include<iostream>
#include<string>
using std::string;
#include<map>
using std::map;
#include<cstdlib>
using std::endl;
#include"ostrm.h"
#include"istrm.h"
using namespace std;

Ostrm::Ostrm(unsigned long nodes) : nodes(nodes)
{
}

Ostrm::~Ostrm()
{
  for( map<string,std::ofstream*>::iterator it = stream.begin();
        it != stream.end(); it++ )
    if( it->second ) {
      if( it->second->is_open() )
        it->second->close();
      delete dynamic_cast<ofstream*>(it->second);
    }
}

void Ostrm::init( Istrm& inputf )
{
  vector<double> temp = inputf.Numbers();
  for( uint i=0; i<temp.size(); i++ )
    Node[i] = temp[i];
  for( uint i=0; i<Node.size(); i++ )
    if( Node[i] > nodes ) {
      std::cerr<<"Trying to plot node "<<Node[i]
          <<", which is bigger than the largest node."<<endl;
      exit(EXIT_FAILURE);
    }
}

void Ostrm::Step(void)
{
  for( map<string,std::ofstream*>::iterator it = stream.begin();
        it != stream.end(); it++ ) {
    for( int i=0; i<<Node.size(); i++ )
      *it->second << trace[it->first] << " ";
    *it->second << endl;
  }
}

void Ostrm::Register( const string& filename, const double* const trace )
{
  std::pair<map<string,const double* const>::iterator,bool> ret;
  ret = this->trace.insert( pair<string,const double* const>(filename,trace) );
  if( ret.second == false ) {
    std::cerr<<"Error trying to plot different traces into the same file "
      <<filename<<"."<<endl;
    exit(EXIT_FAILURE);
  }
  std::ofstream* of = new ofstream(filename.c_str());
  stream.insert( pair<string,std::ofstream*>(filename,of) );
}
