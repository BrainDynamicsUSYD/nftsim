#include"configf.h"
#include"solver.h"

//#include <iostream>
using std::cerr;
using std::endl;
// #include<cstdlib>
#include<cstring>
// #include<string>
using std::string;



int main(int argc, char* argv[])
{
  // print help message
  if( argc>1 )
    for( int i=1; i<argc; i++ )
      if( strcmp(argv[i],"-?")==0 || strcmp(argv[i],"-h")==0
        || strcmp(argv[i],"--help")==0 ) {
      cerr << "NeuroField usage: " << endl
        << "NeuroField [optional switches]" << endl
        << "where the optional switches are:" << endl
        //<< "-d alternate.dump" << endl
        << "-i alternate.conf" << endl
        << "-o alternate.output" << endl
        //<< "-s for not producing dump file (overrides -d switch)" << endl
        << "-v for outputting to standard output"
        << endl;
        return 0;
      }

  // if find keyword "restart" in the argument list, use restart mode
  /*bool restart = false;
  for( int i=0; i<argc-1; i++ )
    if( strcmp(argv[i],"--restart")==0 )
      restart = true;*/

  // open conf file - default is neurofield.conf
  int iconfarg = 0;
  if( argc>2 )
    for( int i=1; i<argc-1; i++)
      if( strcmp(argv[i],"-i") == 0 )
        iconfarg = i + 1;
  const char* confname = iconfarg?argv[iconfarg]:"neurofield.conf";
  //conditional operator condition? if_true:if_false
  
  // initialize inputf, the stream of the configuration file
  Configf* inputf = 0;
  /*if(restart) {
    std::cerr<<"Restart mode is not implemented yet!"<<endl;
    exit(EXIT_FAILURE);
    // istrm = new Restartf(confname);
  }
  else*/
    inputf = new Configf(confname);

  // open file for dumping data for restart - default is neurofield.dump
  /*Dumpf* dumpf = 0;
  int idumparg = 0;
  if( argc>2 )
    for( int i=1; i<argc-1; i++)
      if( strcmp(argv[i],"-d") == 0 )
        idumparg = i + 1;
  dumpf = new Dumpf; dumpf->open(idumparg?argv[idumparg]:"neurofield.dump");*/

  // if given "-s" or "--silent", suppress dumpfile
  /*if( argc>1 )
    for( int i=1; i<argc-1; i++ )
      if( strcmp(argv[i],"-s")==0 || strcmp(argv[i],"--silent")==0 ) {
        if(dumpf) delete dumpf;
        dumpf = 0;
      }*/

  // open file for outputting data - default is neurofield.output
  Dumpf dumpf;
  int ioutarg = 0;
  if( argc>2 )
    for( int i=1; i<argc-1; i++)
      if( strcmp(argv[i],"-o") == 0 )
        ioutarg = i + 1;
  dumpf.open(string(ioutarg?argv[ioutarg]:"neurofield.output"));

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
