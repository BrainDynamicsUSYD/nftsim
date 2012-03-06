/***************************************************************************
                          output.h  -  outputs a field
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef OUTPUT_H
#define OUTPUT_H

#include<string>
using std::string;
#include<vector>
using std::vector;
#include"nf.h"

class Output: public NF
{
  Output(const Output& other); // No copy constructor
  Output(); // No default constructor
  Output& operator=(const Configf& other); // No assignment operator
  string name; // name of field
  const vector<double>& field; // field to output
protected:
  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  Output( const string& name, const vector<double>& field );
  virtual ~Output(void);
  virtual void step(void);
  const string& fieldname(void) const; // returns name

  static vector<int> node; // vector of nodes to output
  static Dumpf dumpf; // file to dump
};

#endif
