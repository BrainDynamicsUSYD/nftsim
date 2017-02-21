/** @file main.cpp
  @brief Implements the main entry point for Neurofield, a multiscale neural field software.

  NeuroField is capable of simulating scales from a few tenths of a millimetre
  and a few milliseconds upward. It allows for the specification of models with:
    + an arbitrary number of neural populations, of different types and with
      different parameters;
    + different types of connections between populations; and,
    + different type of activity propagation, with or without propagation
      time delays between neural populations.

  @author Peter Drysdale, Felix Fung,
*/

// Neurofield headers
#include "configf.h"    // Configf;
#include "dumpf.h"      // Dumpf;
#include "solver.h"     // Solver;

// C++ standard library headers
#include <iostream>  // std::cerr; std::cout; std::endl;
#include <string>    // std::string;
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
        cout << endl << "Usage: NeuroField [optional switches]" << endl
             << endl
             << "  By default NeuroField will read neurofield.conf and write output" << endl
             << "  to neurofield.output" << endl
             << endl
             << "Options:" << endl
             << "  -i, --input [FILE]          Read from a particular configuration" << endl
             << "  -o, --output [FILE]         Write to a particular output file" << endl
             << "  -h, -?, --help              Print this message" << endl
             << "  -v, --verbose               Send output to stdout" << endl << endl
             << "Examples:" << endl
             << "  NeuroField " << endl
             << "  NeuroField -i alternate.conf -o alternate.output" << endl
             << endl
             << "(c) Copyright Complex Systems Group, School of Physics, University of Sydney 2015" << endl
             << "Use of NeuroField indicates your acceptance of the licensing conditions distributed with this end" << endl;
        return 0;
      }
    }
  }

  // open conf file - default is neurofield.conf
  //conditional operator syntax: condition? if_true:if_false
  int iconfarg = 0;
  if( argc>2 ) {
    for( int i=1; i<argc-1; i++) {
      if( strcmp(argv[i],"-i") == 0  || strcmp(argv[i],"--input") == 0 ) {
        iconfarg = i + 1;
      }
    }
  }
  const char* confname = iconfarg != 0?argv[iconfarg]:"neurofield.conf";
  if(iconfarg == 0) {
    cerr << "Warning: Using neurofield.conf for input by default" << endl;
  }
  auto  inputf = new Configf(confname);

  // open file for outputting data - default is neurofield.output
  Dumpf dumpf;
  int ioutarg = 0;
  if( argc>2 ) {
    for( int i=1; i<argc-1; i++) {
      if( strcmp(argv[i],"-o") == 0 || strcmp(argv[i],"--output") == 0) {
        ioutarg = i + 1;
      }
    }
  }
  dumpf.open(string(ioutarg != 0?argv[ioutarg]:"neurofield.output"));
  if(ioutarg == 0) {
    cerr << "Warning: Using neurofield.output for output by default" << endl;
  }

  if( argc>1 ) {
    for( int i=1; i<=argc-1; i++) {
      if( strcmp(argv[i],"-v")==0 || strcmp(argv[i],"--verbose")==0 ) {
        dumpf.verbose();
      }
    }
  }

  // construct, initialize and solve the neural field theory
  Solver neurofield(dumpf);
  *inputf>>neurofield;
  neurofield.solve();

  delete inputf;
  return EXIT_SUCCESS;
}
