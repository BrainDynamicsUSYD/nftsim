/** @file output.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_OUTPUT_H
#define NEUROFIELD_SRC_OUTPUT_H

#include <string>
using std::string;
#include <vector>
using std::vector;

class Outlet {
 public:
  Outlet( string  name, const vector<double>& field, bool single_output );
  const string name;
  const vector<double>& field;
  const bool single_output;
};

class Output {
  vector<Outlet*> outputs;
  const string key;
  string m_prefix;
 public:
  explicit Output( string  key );
  void operator() ( const string& name, const vector<double>& field );
  void prefix( const string& object, vector<double>::size_type index );
  void operator() ( const string& object, vector<double>::size_type index,
                    const string& name, const vector<double>& field );
  void singleNode( const string& name, const vector<double>& field );
  void singleNode(
    const string& object, vector<double>::size_type index, const string& name, const vector<double>& field );
  bool empty(void) const;
  operator vector<Outlet*>(void) const;
};

#endif //NEUROFIELD_SRC_OUTPUT_H
