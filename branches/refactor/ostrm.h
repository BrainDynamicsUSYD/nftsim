/***************************************************************************
                          ostrm.h  -  outputs traces
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef OSTRM_H
#define OSTRM_H

#include<fstream>
#include<iostream>
#include<string>
using std::string;
#include<vector>
using std::vector;
#include<map>
using std::map;
#include"configf.h"

class Ostrm
{
public:
  Ostrm(long nodes);
  ~Ostrm();
  void Step(void); // outputs all traces to their corresponding stream
  // register a trace to a stream
  void Register( const string& filename, const double* const trace );
  void init( Configf& inputf );
private:
  Ostrm(const Ostrm& other); // No copy constructor
  Ostrm(); // No default constructor
  Ostrm& operator=(const Ostrm& other); // No assignment operator
  long nodes; // number of nodes
  vector<long> Node; // list of nodes to output
  map<string,const double* const> trace;
  map<string,std::ofstream*> stream;
  friend void operator>>( Configf& Configf, Ostrm& ostrm );
};

#endif
