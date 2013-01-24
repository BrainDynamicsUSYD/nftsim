#ifndef OUTPUT_H
#define OUTPUT_H

#include<string>
using std::string;
#include<vector>
using std::vector;
#include"dumpf.h"

class Outlet
{
  const string name; // name of field
  const vector<double>& field; // field to output
  const bool single_output;
public: 
  Outlet( const string& name, const vector<double>& field, bool single_output=false );
  const string& fieldname(void) const; // returns name
  void step(void) const; // output field values
  void writeName(void) const;
  void writeNode(void) const;

  static vector<int> node; // vector of nodes to output
  static Dumpf dumpf; // file to dump
};

class Output
{
  vector<Outlet*> outputs;
  const string key;
  string m_prefix;
public:
  Output( const string& key );
  void operator() ( const string& name, const vector<double>& field, bool single_output=false );
  void prefix( const string& object, int index );
  void operator() ( const string& object, int index,
          const string& name, const vector<double>& field, bool single_output=false );
  bool empty(void) const;
  operator vector<Outlet*>(void) const;
};

#endif
