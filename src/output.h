/** @file output.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_OUTPUT_H
#define NEUROFIELD_SRC_OUTPUT_H

// C++ standard library headers
#include <string> //std::string;
#include <vector> //std::vector;

class Outlet {
 public:
  Outlet( std::string  name, const std::vector<double>& field, bool single_output );
  const std::string name;
  const std::vector<double>& field;
  const bool single_output;
};

class Output {
  std::vector<Outlet*> outputs;
  const std::string key;
  std::string m_prefix;
 public:
  explicit Output( std::string  key );
  void operator() ( const std::string& name, const std::vector<double>& field );
  void prefix( const std::string& object, std::vector<double>::size_type index );
  void operator() ( const std::string& object, std::vector<double>::size_type index,
                    const std::string& name, const std::vector<double>& field );
  void singleNode( const std::string& name, const std::vector<double>& field );
  void singleNode( const std::string& object, std::vector<double>::size_type index,
                   const std::string& name, const std::vector<double>& field );
  bool empty(void) const;
  operator std::vector<Outlet*>(void) const;
};

#endif //NEUROFIELD_SRC_OUTPUT_H
