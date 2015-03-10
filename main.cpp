#include<cstring>
#include"solver.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

int main(int argc, char* argv[])
{
  // print help message
  if( argc>1 )
    for( int i=1; i<argc; i++ )
      if( strcmp(argv[i],"-?")==0 || strcmp(argv[i],"-h")==0
        || strcmp(argv[i],"--help")==0 ) {
      cout << endl << "Usage: NeuroField [optional switches]" << endl
        << endl
        << "  Numerical integrator for neural field models." << endl
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
        << "Use of NeuroField indicates your acceptance of the licensing conditions distributed with this program" << endl;
        return 0;
      }

  // open conf file - default is neurofield.conf
  //conditional operator syntax: condition? if_true:if_false
  int iconfarg = 0;
  if( argc>2 )
    for( int i=1; i<argc-1; i++)
      if( strcmp(argv[i],"-i") == 0  || strcmp(argv[i],"--input") == 0 )
        iconfarg = i + 1;
  const char* confname = iconfarg?argv[iconfarg]:"neurofield.conf";
  if(!iconfarg){
      cerr << "Warning: Using neurofield.conf for input by default" << endl;
  }
  Configf* inputf = new Configf(confname);
  
  // open file for outputting data - default is neurofield.output
  Dumpf dumpf;
  int ioutarg = 0;
  if( argc>2 )
    for( int i=1; i<argc-1; i++)
      if( strcmp(argv[i],"-o") == 0 || strcmp(argv[i],"--output") == 0)
        ioutarg = i + 1;
  dumpf.open(string(ioutarg?argv[ioutarg]:"neurofield.output"));
  if(!ioutarg){
      cerr << "Warning: Using neurofield.output for output by default" << endl;
  }

  if( argc>1 )
    for( int i=1; i<=argc-1; i++)
      if( strcmp(argv[i],"-v")==0 || strcmp(argv[i],"--verbose")==0 )
        dumpf.verbose();

  // construct, initialize and solve the neural field theory
  Solver neurofield(dumpf); *inputf>>neurofield;
  neurofield.solve();

  delete inputf;
  return EXIT_SUCCESS;
}
