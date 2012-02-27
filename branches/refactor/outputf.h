/***************************************************************************
                          outputf.h  -  outputs a field into an ofstream
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef OUTPUTF_H
#define OUTPUTF_H

#include<string>
using std::string;
#include<vector>
using std::vector;
#include"nf.h"

class Outputf: public NF
{
  Outputf(const Configf& other); // No copy constructor
  Outputf(); // No default constructor
  Outputf& operator=(const Configf& other); // No assignment operator
  vector<int> node; // vector of nodes to output
  string name; // name of field
  const vector<double>& field; // field to output
protected:
  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  Outputf( const vector<int>& node,
      const string& name, const vector<double>& field );
  virtual ~Outputf(void);
  virtual void step(void);
};

#endif
