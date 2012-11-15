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
public: 
  Outlet( const string& name, const vector<double>& field );
  const string& fieldname(void) const; // returns name
  void step(void) const; // output field values

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
  void operator() ( const string& name, const vector<double>& field );
  void prefix( const string& object, int index );
  void operator() ( const string& object, int index,
          const string& name, const vector<double>& field );
  bool empty(void) const;
  operator vector<Outlet*>(void) const;
};

#endif
