/** @file configf.h
  @brief Declaration of the class used to read information from `.conf` files.

  The Configf class is derived from std::ifstream, it provides additional member
  functions to help parse NFTsim's `.conf` files.

  Configuration file grammar, as assumed by these functions:

  - The file starts with a free-text comment, which runs up to the first
    occurrence of the text `Time:` (the first entry). The comment may contain
    any text, including ':', except `Time:` itself.
  - After the comment the file is a sequence of entries `Key: value ...`,
    separated by any whitespace. A key is the text immediately before a ':'.
    Keys may contain spaces (eg `Longside nodes`), so a key is recognised by
    comparing the text before a ':' with the key being looked for. A key
    matches only as a whole: it must end at the ':' and must not be preceded
    by a letter, digit or '_' (so `nu` does not match `Xnu:`, and
    `Coupling 1` does not match `Coupling 11:`).
  - Object labels start sections. `Population N:`, `Propagator N:`,
    `Coupling N:` (N a positive integer, written after exactly one space) and
    `Output:` are top-level labels; each section runs from its label to the
    next top-level label or the end of the file. `Dendrite N:` labels a
    subsection of a population, which runs to the next `Dendrite` label or
    the end of the enclosing section. A `Stimulus:` entry is not a label: its
    parameters belong to the enclosing population's section.
  - The entries between the comment and `Population 1:` (Time, Deltat, Nodes,
    the connection matrix, ...) form the global section, named "".

  Two styles of access are provided. param(), optional() and next() read
  entries sequentially from the current stream position, so entries must be
  in the expected order. lookup() (and the older find()) searches a named
  section of the file, independently of the stream position.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_CONFIGF_H
#define NFTSIM_SRC_CONFIGF_H

// C++ standard library headers
#include <fstream>  // std::ifstream; tellg; seekg;
#include <iostream> // std::endl; std::cerr; std::streamsize; std::ios::beg;
#include <string>   // std::string;
#include <vector>   // std::vector;


class Configf : public std::ifstream { // derived class
  char* buffer;
  std::streamsize filesize;
  std::string content;       ///< Whole file, as read on construction.
  size_t content_start;      ///< Offset of the end of the leading comment.
  std::vector<size_t> colons; ///< Offsets of every ':' after the comment.

  /// True if the text just before content[colon] is exactly the key `key`.
  bool keyBefore( size_t colon, const std::string& key ) const;
  /// Level of the section label ending at content[colon]: 1 for a top-level
  /// label, 2 for a Dendrite label, 0 if the key there is not a label. On
  /// success `start` is set to the offset of the label's first character.
  int labelLevel( size_t colon, size_t& start ) const;
  /// Locate section `section`: [begin,end) is the text after its label.
  void sectionBounds( const std::string& section, size_t& begin, size_t& end ) const;
  /// First whitespace-delimited token starting at or after `pos`, if it
  /// starts before `end`; otherwise "".
  std::string tokenAt( size_t pos, size_t end ) const;
  /// Print the line of the file holding offset `pos`, to help locate errors.
  void printContext( size_t pos ) const;
 public:
  Configf(const Configf&) = delete;  ///< No copy constructor allowed.
  Configf() = delete;                ///< No default constructor allowed.
  Configf& operator=(const Configf&) = delete; ///< No copy assignment operator allowed.


  explicit Configf( const std::string filename ); // const string of filename for ifstream
  ~Configf() override;

  /// Looks for the next parameter called "param" and stores it in T.
  /** If "param" is not found, terminate program. */
  template<class T> void param(const std::string& param, T& ret, int delim=':' );

  /// Looks for the next parameter called "param" and stores it in T
  /** If "param" is not found, return false. */
  template<class T> bool optional( const std::string& param, T& ret, int delim=':' );

  /// Read & return an arbitrary array of doubles
  std::vector<double> numbers();

  /// Return all whitespace separated strings before delimiting string
  std::vector<std::string> arb( const std::string& delim );

  /// Value of entry `key` in section `section` (see the grammar above).
  /** Returns the first whitespace-delimited token after the first `key:`
      within the section, or, if `key` is empty, the token after the section
      label itself (the object type, eg "Map" in `Coupling 3: Map - nu: 1`).
      `section` is an exact label such as "Population 2", "Coupling 11" or
      "Dendrite 3", or "" for the global section. Exits with an error if the
      section does not exist. If the key is absent from the section, returns
      "" when `required` is false, and exits with an error naming the section
      and key when it is true. The stream position is not changed. */
  std::string lookup( const std::string& section, const std::string& key,
                      bool required ) const;

  /// Older form of lookup(): find("A*B") is lookup("A", "B", false).
  /** A trailing ':' on either part is ignored, and find("A") (no '*') is
      lookup("A", "", false), which returns the object type. Unlike earlier
      versions, the search is confined to section A. */
  std::string find( const std::string& Check ) const;

  /// Points to next delim, and verify it is "check"+"delim"
  bool next( const std::string& Check, int delim=':' );

  /// Moves the stream to the first entry with key `keyword` after the comment.
  void go2( const std::string& keyword );

  std::streamsize tell() {
    return std::ifstream::tellg();
  }
  void seek( std::streamsize position ) {
    std::ifstream::seekg(position);
  }

  friend class Dumpf;
};

/// global function that returns string=="Object#" for config file parsing
/** also useful in naming outputf "solution.phi.#". */
std::string label( const std::string& prefix, size_t index );

template<class T> void Configf::param(const std::string& param, T& ret, int delim ) {
  if( next(param,delim) ) {
    if( !good() ) {
      std::cerr << "Input variable '" << param << "' is given a wrong type."
           << std::endl;
      exit(EXIT_FAILURE);
    }
    *this >> ret;
  } else {
    std::streamsize position = tellg();
    seekg(0,std::ios::beg);
    read(buffer,position);
    buffer[position] = '\0';
    std::cerr << "Unable to find next input variable: "
         << param << std::endl
         << "From configuration file:" << std::endl
         << buffer << std::endl;
    size_t global_begin;
    size_t global_end;
    sectionBounds("", global_begin, global_end);
    if( static_cast<size_t>(position) < global_end ) {
      std::cerr << "(The free-text comment at the top of a configuration file"
                << " runs up to the first 'Time:', which must be the first"
                << " entry; the comment must not itself contain 'Time:'.)"
                << std::endl;
    }
    exit(EXIT_FAILURE);
  }
}

template<> inline void Configf::param<std::string>(const std::string& param,
                                                   std::string& ret, int delim ) {
  if( next(param,delim) ) {
    *this >> buffer;
    if( !good() ) {
      std::cerr << "Input variable '" << param << "' is given a wrong type."
           << std::endl;
      exit(EXIT_FAILURE);
    }
    ret = std::string(buffer);
  } else {
    std::cerr << "Unable to find next input variable: " << param << std::endl;
    printContext( static_cast<size_t>(tellg()) );
    exit(EXIT_FAILURE);
  }
}

template<class T> bool Configf::optional( const std::string& param, T& ret, int delim ) {
  if( next(param,delim) ) {
    *this >> ret;
    if( !good() ) {
      std::cerr << "Input variable '" << param << "' is given a wrong type."
           << std::endl;
      exit(EXIT_FAILURE);
    }
    return true;
  }
  return false;
}

#endif //NFTSIM_SRC_CONFIGF_H
