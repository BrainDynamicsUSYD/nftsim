/** @file output.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_OUTPUT_H
#define NFTSIM_SRC_OUTPUT_H

// C++ standard library headers
#include <memory> // std::unique_ptr;
#include <string> // std::string;
#include <vector> // std::vector;

/// One field to write to the output file: its column name and a reference
/// to the vector that holds its current values.
class Outlet {
 public:
  Outlet( std::string  name, const std::vector<double>& field, bool single_output );
  const std::string name;
  const std::vector<double>& field;
  const bool single_output;
};

/// Collects the Outlets an object offers for a requested output key.
/** Solver::Outputs passes an Output to an object's output() member, which
    calls operator() (or singleNode()) for each field it can write. Fields
    whose name matches the key (or all fields, if the key is empty) become
    Outlets, which Solver::Outputs then takes with release(). */
class Output {
  std::vector<std::unique_ptr<Outlet>> outputs;
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
  bool empty() const;
  /// Hands over the collected Outlets, leaving this Output empty.
  std::vector<std::unique_ptr<Outlet>> release();
};

#endif //NFTSIM_SRC_OUTPUT_H
