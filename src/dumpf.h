/** @file dumpf.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_DUMPF_H
#define NFTSIM_SRC_DUMPF_H

// Other nftsim headers
#include "configf.h" // Configf;

// C++ standard library headers
#include <iostream> // std::istream; std::ostream; std::ofstream;
#include <string>   // std::string;

class Dumpf {
  std::ofstream file;  ///< stream to a file, if outputting to file
  std::ostream* s;     ///< pointer to output, maybe to file, or to cout
  std::string filename;
 public:
  Dumpf(const Dumpf&) = delete; // No copy constructor allowed.
  Dumpf& operator=(const Dumpf&) = delete; // No assignment operator allowed.

  Dumpf();
  ~Dumpf();
  void outputConfig( Configf& configf );
  void open( const std::string& filename );
  void verbose();
  Dumpf& operator<< ( double f );
  Dumpf& operator<< ( const std::string& str );
  Dumpf& operator<< ( int i );
  Dumpf& operator<< ( std::ostream& (*pf)(std::ostream&) ); // for manupulators eg endl
};

std::ostream& septor( std::ostream& os ); ///< implements the separator "|" between fields
std::ostream& space( std::ostream& os );  ///< puts a space into dumpfile
std::ostream& setw( std::ostream& os );

#endif
