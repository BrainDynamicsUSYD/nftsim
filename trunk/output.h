#ifndef OUTPUT_H
#define OUTPUT_H

#include<string>
using std::string;
#include<vector>
using std::vector;
#include"dumpf.h"

class Output
{
  const string name; // name of field
  const vector<double>& field; // field to output
public: 
  Output( const string& name, const vector<double>& field );
  const string& fieldname(void) const; // returns name
  void step(void) const; // output field values

  static vector<int> node; // vector of nodes to output
  static Dumpf dumpf; // file to dump
};

#endif
