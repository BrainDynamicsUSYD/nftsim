/** @file configf.cpp
  @brief Definition  of the class used to read information from `.conf` files.

  The Configf class is derived from std::ifstream, it provides additional member
  functions to help parse NFTsim's `.conf` files.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "configf.h"    // Configf;

// C++ standard library headers
#include <cctype>   // std::isalnum; std::isdigit; std::isspace;
#include <iostream> // std::streamoff; std::ios::beg; std::ios::end; std::streampos; std::ifstream;
#include <sstream>  // std::stringstream;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

Configf::Configf( const string filename )
  : std::ifstream(filename) {
  if( !*this ) {
    cerr << "Unable to open configuration file." << endl;
    exit(EXIT_FAILURE);
  }
  std::streamoff sp = tellg(); ///< stores current file position
  seekg(0,std::ios::end);
  filesize = tellg();
  buffer = new char[filesize+1];
  buffer[filesize] = '\0';
  seekg(0,std::ios::beg);
  read(buffer,filesize);
  content = string(buffer);
  clear();

  // The leading free-text comment runs up to the first "Time:", the first
  // entry (see configf.h). Start reading there, so the comment may contain
  // ':'. In a file whose comment has no ':', the first ':' follows "Time", so
  // this changes nothing for such files. If there is no "Time:" at all, start
  // at the beginning and let the read of "Time" report the error.
  content_start = content.find("Time:");
  if( content_start == string::npos ) {
    content_start = 0;
  }
  for( size_t i = content.find(':', content_start); i != string::npos;
       i = content.find(':', i+1) ) {
    colons.push_back(i);
  }
  seekg( sp + static_cast<std::streamoff>(content_start) );
}

Configf::~Configf() {
  delete[] buffer;
}

/// Reads an arbitrary number of doubles and return it in an array
vector<double> Configf::numbers() {
  vector<double> ret;
  while( good() ) { // read until non-numeral
    double buffer;
    *this >> buffer;
    ret.push_back( buffer );
  }
  ret.pop_back(); // deletes last element==0
  clear(); // clear the fail bit
  return ret;
}

vector<string> Configf::arb( const string& delim ) {
  string temp;
  vector<string> ret_value;
  while( temp!=delim && *this ) {
    *this >> temp;
    ret_value.push_back(temp);
    if( bad() ) {
      cerr<<"Error reading configuration file while reading arbitrary strings."
          <<endl;
      exit(EXIT_FAILURE);
    }
  }
  ret_value.pop_back();
  for( size_t i=0; i<delim.size(); i++ ) {
    unget();
  }
  return ret_value;
}

namespace {
/// Characters that may form part of a word in a key.
bool isWordChar( char c ) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}
bool isSpace( char c ) {
  return std::isspace(static_cast<unsigned char>(c)) != 0;
}
} // namespace

bool Configf::keyBefore( size_t colon, const string& key ) const {
  if( key.empty() || colon < content_start + key.size() ) {
    return false;
  }
  size_t start = colon - key.size();
  if( content.compare(start, key.size(), key) != 0 ) {
    return false;
  }
  return start == content_start || !isWordChar(content[start-1]);
}

int Configf::labelLevel( size_t colon, size_t& start ) const {
  if( keyBefore(colon, "Output") ) {
    start = colon - 6;
    return 1;
  }
  // "<Word> <digits>" immediately before the ':'
  size_t d = colon;
  while( d > content_start && std::isdigit(static_cast<unsigned char>(content[d-1])) != 0 ) {
    d--;
  }
  if( d == colon || d == content_start || content[d-1] != ' ' ) {
    return 0;
  }
  static const char* const top[] = {"Population", "Propagator", "Coupling"};
  for( const char* word : top ) {
    if( keyBefore(d-1, word) ) {
      start = d - 1 - string(word).size();
      return 1;
    }
  }
  if( keyBefore(d-1, "Dendrite") ) {
    start = d - 1 - 8;
    return 2;
  }
  return 0;
}

void Configf::sectionBounds( const string& section, size_t& begin,
                             size_t& end ) const {
  size_t k = 0; // index into colons of the section's label
  int level = 1;
  size_t start = 0;
  if( section.empty() ) { // global section: up to the first label
    begin = content_start;
  } else {
    for( ; k < colons.size(); k++ ) {
      if( keyBefore(colons[k], section) ) {
        break;
      }
    }
    if( k == colons.size() ) {
      cerr << "Configuration error: the section '" << section
           << "' was not found (expected an entry '" << section << ":')."
           << endl;
      exit(EXIT_FAILURE);
    }
    level = labelLevel(colons[k], start);
    if( level == 0 ) {
      cerr << "Configuration error: '" << section
           << "' is not a section label." << endl;
      exit(EXIT_FAILURE);
    }
    begin = colons[k] + 1;
    k++;
  }
  end = content.size();
  for( ; k < colons.size(); k++ ) {
    if( colons[k] < begin ) {
      continue;
    }
    int next_level = labelLevel(colons[k], start);
    if( next_level != 0 && (section.empty() || next_level <= level) ) {
      end = start;
      return;
    }
  }
}

string Configf::tokenAt( size_t pos, size_t end ) const {
  while( pos < content.size() && isSpace(content[pos]) ) {
    pos++;
  }
  if( pos >= end ) {
    return string("");
  }
  size_t stop = pos;
  while( stop < content.size() && !isSpace(content[stop]) ) {
    stop++;
  }
  return content.substr(pos, stop-pos);
}

void Configf::printContext( size_t pos ) const {
  if( pos > content.size() ) {
    return;
  }
  size_t line = 1;
  for( size_t i = 0; i < pos; i++ ) {
    if( content[i] == '\n' ) {
      line++;
    }
  }
  size_t a = content.rfind('\n', pos == 0 ? 0 : pos-1);
  a = (a == string::npos || pos == 0) ? 0 : a+1;
  size_t b = content.find('\n', pos);
  if( b == string::npos ) {
    b = content.size();
  }
  cerr << "Near line " << line << " of the configuration file:" << endl
       << "  " << content.substr(a, b-a) << endl;
}

string Configf::lookup( const string& section, const string& key,
                        bool required ) const {
  size_t begin;
  size_t end;
  sectionBounds(section, begin, end);
  if( key.empty() ) {
    return tokenAt(begin, end);
  }
  for( size_t colon : colons ) {
    if( colon <= begin ) {
      continue;
    }
    if( colon >= end ) {
      break;
    }
    if( keyBefore(colon, key) && colon - key.size() >= begin ) {
      return tokenAt(colon+1, end);
    }
  }
  if( required ) {
    cerr << "Configuration error: no entry '" << key << ":' in "
         << (section.empty() ? string("the global section (before 'Population 1:')")
                             : "section '" + section + "'") << "." << endl;
    printContext( section.empty() ? begin : begin-1 );
    exit(EXIT_FAILURE);
  }
  return string("");
}

string Configf::find( const string& Check ) const {
  if( Check.empty() ) {
    cerr << "Attempted to use Configf::find searching for an empty string"
         << endl;
    exit(EXIT_FAILURE);
  }
  size_t wildcard_pos = Check.find('*');
  string section = Check.substr(0, wildcard_pos);
  string key = (wildcard_pos == string::npos) ? string("")
                                               : Check.substr(wildcard_pos+1);
  if( !section.empty() && section[section.size()-1] == ':' ) {
    section.erase(section.size()-1);
  }
  if( !key.empty() && key[key.size()-1] == ':' ) {
    key.erase(key.size()-1);
  }
  return lookup(section, key, false);
}

bool Configf::next( const string& Check, int delim ) {
  if( Check.empty() ) {
    cerr << "Configf validating an empty string." << endl;
    exit(EXIT_FAILURE);
  }
  std::streampos sp = tellg();
  getline( buffer, filesize, delim );
  if( !good() ) {
    cerr << "Error reading config file when looking for: " << Check << endl;
    exit(EXIT_FAILURE);
  }
  string param(buffer);
  int pos = param.find( Check, param.length()-Check.length() );
  if( pos == -1 ) {
    seekg(sp);
    return false;
  }
  return true;
}

void Configf::go2( const string& keyword ) {
  if( keyword.empty() ) {
    cerr << "Searching for an empty string in configuration file." << endl;
    exit(EXIT_FAILURE);
  }
  for( size_t colon : colons ) {
    if( keyBefore(colon, keyword) ) {
      clear();
      seekg( static_cast<std::streamoff>(colon - keyword.size()) );
      return;
    }
  }
  cerr << "Configuration reading error: keyword '" << keyword
       << ":' not found." << endl;
  exit(EXIT_FAILURE);
}

string label( const string& prefix, size_t index ) {
  stringstream ss;
  ss<<prefix.c_str()<<index;
  return ss.str();
}
