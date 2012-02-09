/***************************************************************************
                          main.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by PeterDrysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<fstream>
using std::ofstream;
using std::ios;
#include <iostream>
using std::endl;
#include<iomanip>
using std::setprecision;
#include<cstdlib>
#include<cstring>

#include"istrm.h"
#include"connectmat.h"
#include"propagnet.h"
#include"poplist.h"

int main(int argc, char* argv[])
{
  // Print help message
  if( argc>2 )
    for( int i=1; i<argc-1; i++ )
      if( strcmp(argv[i],"-?")==0 || strcmp(argv[i],"-h")==0
        || strcmp(argv[i],"--help")==0 ) {
      std::cerr << "NeuroField usage: " << endl
        << "NeuroField [optional switches]" << endl
        << "where the optional switches are" << endl
        << "-d alternate.dump" << endl
        << "-i alternate.conf" << endl
        << "-o alternate.output" << endl;
        return 0;
      }

  // Open file for dumping data for restart -default is neurofield.dump
  int idumparg = 0; // Index No. of dump file name in argv
  if( argc>2 )
    for( int i=1; i<argc-1; i++)
      if( strcmp(argv[i],"-d") == 0 )
        idumparg = i + 1;
  ofstream dumpf(idumparg?argv[idumparg]:"neurofield.dump",ios::out);
  if( !dumpf ) {
    std::cerr << "Unable to open "
      << (idumparg?argv[idumparg]:"neurofield.dump") << " for output.\n";
      exit(EXIT_FAILURE);
  }
  dumpf << setprecision(14);

  // Open conf file - default is neurofield.conf
  int iconfarg = 0;
  if( argc>2 )
    for( int i=1; i<argc-1; i++)
      if( strcmp(argv[i],"-i") == 0 )
        iconfarg = i + 1;
  Istrm inputf(iconfarg?argv[iconfarg]:"neurofield.conf");

  // Parse in global parameters from conf file
  // Anything before Nodes per population is ignored as comment
  long Nodes; inputf.Param("Nodes",Nodes);
  dumpf << "Dump file for NeuroField" << endl
    << "Nodes: " << Nodes << endl;
  int nSteps; inputf.Param("Integration steps",nSteps);
  dumpf << "Integration steps :" << nSteps;
  long nSkip=0;
  if(inputf.Optional("Skippoints",nSkip))
    dumpf << " Skippoints: " << nSkip << " ";
  double deltat; inputf.Param("Deltat",deltat);
  dumpf << " Deltat: " << deltat << endl;

  // glutamate dynamics is loaded by Couplinglist
  inputf.ignore(':'); inputf.ignore(':');

  // Read in connection matrix, determine number of populations and connections
  inputf.ignore(':'); //Param("Connection matrix");
  int nPop; int nCnt; ConnectMat connectmat(inputf,nPop,nCnt);
  connectmat.dump(dumpf);

  // Construct the classes
  Poplist poplist(Nodes,nPop,connectmat);
  PropagNet propagnet(deltat,Nodes,nPop,nCnt,inputf,dumpf);
  bool restart = false;
  for( int i=0; i<argc-1; i++ )
    if( strcmp(argv[i],"restart")==0 )
      restart = true;
  if(restart) { // restart mode
      poplist.restart(inputf,propagnet,connectmat);
      propagnet.restart(inputf,poplist,connectmat);
  }   
  else { // normal mode
    poplist.init(inputf,propagnet,connectmat);
    propagnet.init(inputf,poplist,connectmat);
  }

  // Open file for outputting data 
  int ioutarg = 0;
  if( argc>2 )
    for(int i=1;i<(argc-1);i++)
      if(strcmp(argv[i],"-o")==0)
        ioutarg = i + 1;
  ofstream outputf( (ioutarg?argv[ioutarg]:"neurofield.output"),ios::out);
  if( !outputf ) {
    std::cerr << "Unable to open "
      << (ioutarg?argv[ioutarg]:"neurofield.output") << " for output.\n";
    exit(EXIT_FAILURE);
  }

  // Initialize the output routine
  if(nSkip!=0) outputf << "Skippoints: " << nSkip << " ";
  outputf << "Deltat: " << deltat << endl;
  outputf << "Number of integration steps:" << nSteps << endl;
  propagnet.initoutput(inputf,outputf,nCnt,Nodes);

  //  Main integration Loop
  long skip=nSkip;
  for(int k=1;k<nSteps+1;k++){
    propagnet.stepQtoP(poplist,connectmat);
    poplist.stepPops(deltat);
    if(0==skip){
      propagnet.output(outputf);
      skip=nSkip;
    } else {
      skip--;
    }
  }

  // Dump data for restart
  poplist.dump(dumpf);
  propagnet.dump(dumpf);
  propagnet.dumpoutput(dumpf);

  return EXIT_SUCCESS;
}
