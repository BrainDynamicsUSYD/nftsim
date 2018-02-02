/** @file main.cpp
  @brief Implements the main entry point for NFTsim, a multiscale neural field software.

  NFTsim is capable of simulating scales from a few tenths of a millimetre
  and a few milliseconds upward. It allows for the specification of models with:
    + an arbitrary number of neural populations, of different types and with
      different parameters;
    + different types of connections between populations; and,
    + different type of activity propagation, with or without propagation
      time delays between neural populations.

  @author Peter Drysdale, Felix Fung,
*/

// NFTsim headers
#include "configf.h"    // Configf;
#include "dumpf.h"      // Dumpf;
#include "solver.h"     // Solver;

// C++ standard library headers
#include <ctime>     // std::time; std::localtime
#include <iomanip>   // std::setfill; std::setw;
#include <iostream>  // std::cerr; std::cout; std::endl;
#include <string>    // std::string; std::to_string;
#include <sstream>   // std::ostringstream;
using std::cerr;
using std::cout;
using std::endl;
using std::string;

#include <cstring>

int main(int argc, char* argv[]) {
  // print help message
  if( argc>1 ) {
    for( int i=1; i<argc; i++ ) {
      if( strcmp(argv[i],"-?")==0 || strcmp(argv[i],"-h")==0
          || strcmp(argv[i],"--help")==0 ) {
        cout << endl << "Usage: nftsim [optional switches]" << endl
             << endl
             << "  By default NFTsim will read nftsim.conf and write output" << endl
             << "  to nftsim.output" << endl
             << endl
             << "Options:" << endl
             << "  -i, --input [FILE]         Read from a particular configuration;" << endl
             << "  -o, --output [FILE]        Write to a particular output file;" << endl
             << "  -t, --time-stamp           Add time-stamp to auto-generated output file name;" << endl
             << "  -h, -?, --help             Print this message;" << endl
             << "  -v, --verbose              Send output to stdout." << endl << endl
             << "Examples:" << endl
             << "  nftsim " << endl
             << "  nftsim -i alternate.conf -o alternate.output" << endl
             << endl
             << "(c) Copyright Complex Systems Group, School of Physics, University of Sydney 2015" << endl
             << "Use of NFTsim indicates your acceptance of the licensing conditions distributed with this end" << endl;
        return 0;
      }
    }
  }

  // open conf file - default is nftsim.conf
  //conditional operator syntax: condition? if_true:if_false
  int iconfarg = 0;
  if( argc>2 ) {
    for( int i=1; i<argc-1; i++) {
      if( strcmp(argv[i],"-i") == 0  || strcmp(argv[i],"--input") == 0 ) {
        iconfarg = i + 1;
      }
    }
  }
  const string confname = string(iconfarg != 0?argv[iconfarg]:"nftsim.conf");
  if(iconfarg == 0) {
    cerr << "Warning: Using nftsim.conf for input by default" << endl;
  }
  auto inputf = new Configf(confname);

  // Current local time, optionally appended to auto-generated output file name.
  // The time-stamp format used is ISO 8601: YYYY-mm-ddThhMMSS.
  string current_time = "";
  if( argc>1 ) {
    for( int i=1; i<=argc-1; i++) {
      if( strcmp(argv[i],"-t")==0 || strcmp(argv[i],"--time-stamp")==0 ) {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream date_time_stream;
        date_time_stream << "_" << tm.tm_year + 1900 << "-";
        date_time_stream << std::setw( 2 ) << std::setfill( '0' ) << (tm.tm_mon + 1) << "-";
        date_time_stream << std::setw( 2 ) << std::setfill( '0' ) << tm.tm_mday << "T";
        date_time_stream << std::setw( 2 ) << std::setfill( '0' ) << tm.tm_hour;
        date_time_stream << std::setw( 2 ) << std::setfill( '0' ) << tm.tm_min;
        date_time_stream << std::setw( 2 ) << std::setfill( '0' ) << tm.tm_sec;
        current_time = date_time_stream.str();
      }
    }
  }

  // open file for outputting data - default is confname with .conf suffix
  // replaced by .output, so nftsim.conf => nftsim.output.
  Dumpf dumpf;
  int ioutarg = 0;
  if( argc>2 ) {
    for( int i=1; i<argc-1; i++) {
      if( strcmp(argv[i],"-o") == 0 || strcmp(argv[i],"--output") == 0) {
        ioutarg = i + 1;
      }
    }
  }
  const string default_output_name = confname.substr(0, confname.find_last_of(".")) + current_time + ".output";
  dumpf.open(string(ioutarg != 0?argv[ioutarg]:default_output_name));
  if(ioutarg == 0) {
    cerr << "Warning: Using " << default_output_name << " for output by default." << endl;
  }

  if( argc>1 ) {
    for( int i=1; i<=argc-1; i++) {
      if( strcmp(argv[i],"-v")==0 || strcmp(argv[i],"--verbose")==0 ) {
        dumpf.verbose();
      }
    }
  }

  // construct, initialize and solve the neural field theory
  Solver nftsim(dumpf);
  *inputf>>nftsim;
  nftsim.solve();

  delete inputf;
  return EXIT_SUCCESS;
}
