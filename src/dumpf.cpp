/** @file dumpf.cpp
  @brief Defines Dumpf, which writes nftsim's output file (or standard output).

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "dumpf.h"      // Dumpf;

// Other nftsim headers
#include "configf.h"    // Configf;

// C++ standard library headers
#include <algorithm> // std::reverse;
#include <cctype>   // std::tolower;
#include <cfloat>   // FLT_MAX;
#include <cmath>    // HUGE_VALF;
#include <cstdint>  // uint32_t;
#include <cstring>  // std::memcpy;
#include <iomanip>  // std::setw;
#include <iostream> // std::cerr; std::cout; std::ios::beg; std::ostream; std::scientific;
#include <limits>   // std::numeric_limits;
#include <sstream>  // std::ostringstream;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::cout;
using std::endl;
using std::ostream;
using std::scientific;
using std::setw;
using std::string;
using std::vector;

static_assert( std::numeric_limits<double>::is_iec559 && sizeof(double)==8,
               "binary output assumes IEEE 754 64-bit doubles" );
static_assert( std::numeric_limits<float>::is_iec559 && sizeof(float)==4,
               "binary output assumes IEEE 754 32-bit floats" );

namespace {
bool littleEndianHost() {
  const uint32_t one = 1;
  unsigned char first;
  std::memcpy(&first, &one, 1);
  return first == 1;
}

/// Reverses the bytes of each of the n values of size `size` starting at p.
void swapBytes( char* p, size_t n, size_t size ) {
  for( size_t i=0; i<n; i++ ) {
    std::reverse( p + i*size, p + (i+1)*size );
  }
}
} // namespace

bool parseOutputFormat( const string& name, OutputFormat& format ) {
  string lower(name);
  for( char& c : lower ) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  if( lower == "text" ) {
    format = OutputFormat::text;
  } else if( lower == "binary" || lower == "binary64" ) {
    format = OutputFormat::binary64;
  } else if( lower == "binary32" ) {
    format = OutputFormat::binary32;
  } else {
    return false;
  }
  return true;
}

Dumpf::Dumpf() = default;

Dumpf::~Dumpf() {
  file.close();
}

void Dumpf::outputConfig( Configf& configf ) {
  int sp = configf.tellg();
  configf.seekg(0,std::ios::beg);
  configf.read(configf.buffer.get(),configf.filesize);
  configf.seekg(sp);
  s->write(configf.buffer.get(),configf.filesize);
  *this<<endl<<"============================================="<<endl<<endl;
}

void Dumpf::open( const string& filename ) {
  this->filename = filename;
  if( !file.is_open() ) {
    // Binary mode, so that binary output is not altered on platforms that
    // translate line endings (it makes no difference on Linux and macOS).
    file.open( filename.c_str(), std::ios::out | std::ios::binary );
    if( !file ) {
      cerr<<"Error, cannot open file'"<<filename.c_str()<<"' for output."
          <<endl;
      exit(EXIT_FAILURE);
    }
    s = &file;
    file.precision(14);
    file<<scientific;
  } else {
    cerr<<"Attempting to dump into two files."<<endl;
    exit(EXIT_FAILURE);
  }
}

void Dumpf::verbose() {
  if( file.is_open() ) {
    file.close();
  }
  s = &cout;
  s->precision(14);
  cout<<scientific;
}

void Dumpf::format( OutputFormat format, bool fixed ) {
  if( fmt_fixed && !fixed ) {
    return;
  }
  fmt = format;
  fmt_fixed = fmt_fixed || fixed;
}

void Dumpf::binaryHeader( size_t columns, double deltat, double interval ) {
  // A line of key=value pairs, ending in rows=, whose value (the number of
  // rows) finish() writes over the 20-character placeholder "unknown". If the
  // output cannot be rewritten (standard output, a pipe) it is "stream".
  std::ostringstream meta;
  meta << std::scientific;
  meta.precision(16); // 17 significant digits: the doubles read back exactly
  meta << "NFTSIM-BINARY version=1"
       << " dtype=" << (fmt==OutputFormat::binary32 ? "float32" : "float64")
       << " byteorder=little columns=" << columns
       << " deltat=" << deltat << " interval=" << interval << " rows=";
  const std::streamoff start = s->tellp();
  string line = meta.str();
  if( start >= 0 && s == &file ) {
    rows_pos = start + static_cast<std::streamoff>(line.size());
  }
  string placeholder( rows_pos >= 0 ? "unknown" : "stream" );
  placeholder.resize(20, ' ');
  line += placeholder;
  // Pad so that the data start at a multiple of 8 bytes into the file.
  if( start >= 0 ) {
    const std::streamoff end = start + static_cast<std::streamoff>(line.size()) + 1;
    line.append( static_cast<size_t>((8 - end%8)%8), ' ' );
  }
  line += '\n';
  s->write( line.data(), static_cast<std::streamsize>(line.size()) );
}

void Dumpf::binaryRow( const vector<double>& row ) {
  static const bool swap = !littleEndianHost();
  if( fmt == OutputFormat::binary32 ) {
    row32.resize(row.size());
    for( size_t i=0; i<row.size(); i++ ) {
      // Converting a double beyond the float range is undefined, so map it to infinity.
      const double x = row[i];
      row32[i] = x > FLT_MAX ? HUGE_VALF : x < -FLT_MAX ? -HUGE_VALF : static_cast<float>(x);
    }
    if( swap ) {
      swapBytes( reinterpret_cast<char*>(row32.data()), row32.size(), sizeof(float) );
    }
    s->write( reinterpret_cast<const char*>(row32.data()),
              static_cast<std::streamsize>(row32.size()*sizeof(float)) );
  } else if( swap ) {
    row64 = row;
    swapBytes( reinterpret_cast<char*>(row64.data()), row64.size(), sizeof(double) );
    s->write( reinterpret_cast<const char*>(row64.data()),
              static_cast<std::streamsize>(row64.size()*sizeof(double)) );
  } else {
    s->write( reinterpret_cast<const char*>(row.data()),
              static_cast<std::streamsize>(row.size()*sizeof(double)) );
  }
  rows++;
}

void Dumpf::finish() {
  if( rows_pos >= 0 ) {
    string count = std::to_string(rows);
    count.resize(20, ' ');
    file.seekp(rows_pos);
    file.write( count.data(), static_cast<std::streamsize>(count.size()) );
    file.seekp(0, std::ios::end);
  }
  s->flush();
  if( !*s ) {
    cerr << "Error writing output file '" << filename << "'." << endl;
    exit(EXIT_FAILURE);
  }
}

Dumpf& Dumpf::operator<< ( double f ) {
  if( f>=0 ) {
    *s<<" ";

  }*s<<f;
  return *this;
}

Dumpf& Dumpf::operator<< ( const string& str ) {
  *s<<setw(20)<<str.c_str();
  return *this;
}

Dumpf& Dumpf::operator<< ( int i ) {
  *s<<i;
  return *this;
}

Dumpf& Dumpf::operator<< ( ostream& (*pf)(ostream&) ) {
  *s<<pf;
  return *this;
}

ostream& septor( ostream& os ) {
  return os;
}

ostream& space( ostream& os ) {
  os<<" ";
  return os;
}

ostream& setw( ostream& os ) {
  os<<setw(20);
  return os;
}
