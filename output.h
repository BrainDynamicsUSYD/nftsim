#ifndef OUTPUT_H
#define OUTPUT_H

#include<string>
using std::string;
#include<vector>
using std::vector;

class Outlet
{
public: 
  Outlet( const string& name, const vector<double>& field, bool single_output=false );
  const string name;
  const vector<double>& field;
  const bool single_output;
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
