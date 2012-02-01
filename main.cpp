/***************************************************************************
                          main.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by PeterDrysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

// #ifdef HAVE_CONFIG_H
// #include <config.h>
// #endif

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

  if(argc>2){
    for(int i=1;i<(argc-1);i++){
      if( strcmp(argv[i],"-?")==0 || strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0 )
      std::cerr << "NeuroField usage: " << endl
        << "NeuroField [optional switches]" << endl
        << "where the optional switches are" << endl
        << "-d alternate.dump" << endl
        << "-i alternate.conf" << endl
        << "-o alternate.output" << endl

  // Open file for dumping data for restart -default is neurofield.dump
  int idumparg=0; // Index No. of dump file name in argv
  if(argc>2){
    for(int i=1;i<(argc-1);i++){
      if(strcmp(argv[i],"-d")==0){
        idumparg=i+1;
      }
    }
  }
  ofstream dumpf(idumparg?argv[idumparg]:"neurofield.dump",ios::out);
  if( !dumpf ){
    std::cerr << "Unable to open "<< (idumparg?argv[idumparg]:"neurofield.dump") << " for output \n";
    exit(EXIT_FAILURE);
  }
  dumpf << setprecision(14);

  // Open conf file - default is neurofield.conf
  int inamearg=0;
  if(argc>2){
    for(int i=1;i<(argc-1);i++){
      if(strcmp(argv[i],"-i")==0){
        inamearg=i+1;
      }
    }
  }
  Istrm inputf(inamearg?argv[inamearg]:"neurofield.conf"); // open file for reading input data
  if( !inputf ){
    std::cerr << "Unable to open "<< (inamearg?argv[inamearg]:"neurofield.conf") << " for input \n";
    exit(EXIT_FAILURE);
  }

  // Parse in global parameters from conf file
  // Anything before Nodes per population is ignored as comment
  inputf.validate("Nodes per population",58);
  long nodes=0; inputf >> nodes;
  dumpf << "A dump file for the code 'neurofield'" << endl
    << "Nodes per population: " << nodes << endl;
  inputf.validate("Number of neural populations",58);
  int numpops=0; inputf >> numpops;
  dumpf << "Number of neural populations: " << numpops << endl;
  inputf.validate("Number of neural connections",58);
  int numconct=0; inputf >> numconct;
  dumpf << "Number of neural connections: " << numconct << endl;

  if(numconct>(numpops*numpops)){
    std::cerr << "Number of connections cannot exceed the square of the number of populations" << endl;
    exit(EXIT_FAILURE);
  }
  inputf.ignore(200,32); // throwaway remainder of line after Number of connections

  ConnectMat connectmat(numpops,numconct,inputf);
  connectmat.dump(dumpf);

  // Construct the classes
  int nsteps;
  inputf.validate("integration steps",58); 
  inputf >> nsteps;
  dumpf << "Number of integration steps:" << nsteps << " ";
  double deltat;
  long skippts=0;
  if(inputf.optional("Skippoints",58)){
    inputf >> skippts;
    dumpf << "Skippoints:" << skippts << " ";
  }
  inputf.validate("Deltat",58); 
  inputf >> deltat;
  dumpf << "Deltat:" << deltat << " ";
  inputf.ignore(200,32); //throwaway space line before start of populations
  Poplist poplist(nodes,numpops,connectmat);
  PropagNet propagnet(deltat,nodes,numpops,numconct,inputf,dumpf);
  inputf.ignore(200,32); //throwaway space line before start of populations
  if( !(argc>=2 && (strcmp(argv[1],"restart")==0)) ){
    // Read in remaining init parameters and initialize the classes
    poplist.init(inputf,propagnet,connectmat);
    propagnet.init(inputf,poplist,connectmat);
  }   
  else{
    // Read in remaining restart parameters and initialize the classes
    poplist.restart(inputf,propagnet,connectmat);
    propagnet.restart(inputf,poplist,connectmat);
  }

  // Open file for outputting data 
  int ioutarg=0;
  if(argc>2){
    for(int i=1;i<(argc-1);i++){
      if(strcmp(argv[i],"-o")==0){
        ioutarg=i+1;
      }
    }
  }
  ofstream outputf(ioutarg?argv[ioutarg]:"neurofield.output",ios::out);
  if( !outputf ){
    std::cerr << "Unable to open "<< (ioutarg?argv[ioutarg]:"neurofield.output") << " for output \n";
    exit(EXIT_FAILURE);
  }
  // Initialize the output routine
  if(skippts!=0) outputf << "Skippoints: " << skippts << " ";
  outputf << "Deltat: " << deltat << endl;
  outputf << "Number of integration steps:" << nsteps << endl;
  propagnet.initoutput(inputf,outputf,numconct,nodes);

  //  Main integration Loop
  long skip=skippts;
  for(int k=1;k<nsteps+1;k++){
    propagnet.stepQtoP(poplist,connectmat);
    poplist.stepPops(deltat);
    if(0==skip){
      propagnet.output(outputf);
      skip=skippts;
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
