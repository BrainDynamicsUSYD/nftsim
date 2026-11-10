/** @file dumpf.h
  @brief Declares Dumpf, which writes nftsim's output file (or standard output).

  Solver::Outputs writes the configuration, a header of trace names and node
  numbers, and then one row of values per output time through a Dumpf.
  In the default text format numbers are written in scientific notation with
  14 digits after the point. In the binary formats (see OutputFormat) the same
  header is followed by a line of metadata and then raw little-endian rows.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_DUMPF_H
#define NFTSIM_SRC_DUMPF_H

// Other nftsim headers
#include "configf.h" // Configf;

// C++ standard library headers
#include <iostream> // std::istream; std::ostream; std::ofstream;
#include <string>   // std::string;
#include <vector>   // std::vector;

/// Format of the data rows of the output file.
enum class OutputFormat {
  text,     ///< One line of text per output time (the default).
  binary64, ///< Raw little-endian IEEE 754 doubles, one row per output time.
  binary32  ///< As binary64, but each value rounded to a 32-bit float.
};

/// Reads an output format name (case-insensitive): "Text", "Binary" (or
/// "Binary64") or "Binary32". Returns false if `name` is none of these.
bool parseOutputFormat( const std::string& name, OutputFormat& format );

class Dumpf {
  std::ofstream file;  ///< stream to a file, if outputting to file
  std::ostream* s;     ///< pointer to output, maybe to file, or to cout
  std::string filename;
  OutputFormat fmt = OutputFormat::text;
  bool fmt_fixed = false;       ///< Format set on the command line, which the config file cannot change.
  std::streamoff rows_pos = -1; ///< Offset of the rows= value in the binary metadata line, or -1.
  unsigned long long rows = 0;  ///< Binary rows written so far.
  std::vector<float> row32;     ///< Scratch row for OutputFormat::binary32.
  std::vector<double> row64;    ///< Scratch row for byte-swapping on big-endian hosts.
 public:
  Dumpf(const Dumpf&) = delete; // No copy constructor allowed.
  Dumpf& operator=(const Dumpf&) = delete; // No assignment operator allowed.

  Dumpf();
  ~Dumpf();
  void outputConfig( Configf& configf );
  void open( const std::string& filename );
  void verbose();
  /// Sets the output format. A format given with `fixed` true (from the
  /// command line) is kept: later calls with `fixed` false (from the
  /// configuration file's `Format:`) are ignored.
  void format( OutputFormat format, bool fixed );
  OutputFormat format() const { return fmt; }
  bool binary() const { return fmt != OutputFormat::text; }
  /// Ends the text header with the binary metadata line (binary formats only).
  void binaryHeader( size_t columns, double deltat, double interval );
  /// Writes one row of a binary format.
  void binaryRow( const std::vector<double>& row );
  /// Fills in the number of rows in the binary metadata line, if the output
  /// is a file. Call once, after the last row.
  void finish();
  Dumpf& operator<< ( double f );
  Dumpf& operator<< ( const std::string& str );
  Dumpf& operator<< ( int i );
  Dumpf& operator<< ( std::ostream& (*pf)(std::ostream&) ); // for manupulators eg endl
};

std::ostream& septor( std::ostream& os ); ///< separator between groups of fields; prints nothing (it used to print "|")
std::ostream& space( std::ostream& os );  ///< puts a space into dumpfile
std::ostream& setw( std::ostream& os );

#endif
