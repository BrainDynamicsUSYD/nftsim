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
using std::ifstream;
using std::ofstream;
using std::ios;
#include <iostream>
using std::cerr;
using std::endl;
#include <stdlib.h>

#include"connectmat.h"
#include"propagnet.h"
#include"poplist.h"

void readglobalparams(ifstream& datainf, ofstream& dumpf, long& totalnodes, int& numpops, int& numconct);

int main(int argc, char* argv[])
{
//
// Open file for dumping data for restart
//
  ofstream dumpf("eegcode.dump",ios::out);
  if( !dumpf )
    cerr << "Unable to open 'eegcode.dump' for output \n";
//
// Read in the global simulation parameters
//
  long totalnodes=0;
  int numpops=0;
  int numconct=0;
  ifstream inputf("eegcode.conf",ios::in); //open file for reading input data
  if( !inputf )
    cerr << "Unable to open 'eegcode.conf' for input \n";
  readglobalparams(inputf, dumpf, totalnodes, numpops, numconct);
  ConnectMat connectmat(numpops,numconct);
  connectmat.init(inputf);
  connectmat.dump(dumpf);
//
// Construct the classes
//
  int nsteps;
  inputf.ignore(200,58); // throw away everything uptil colon character
  inputf >> nsteps;
  dumpf << "Number of integration steps :" << nsteps << " ";
  float deltat, deltax;
  inputf.ignore(200,58); // throw away everything uptil colon character
  inputf >> deltat;
  dumpf << "Deltat :" << deltat << " ";
  inputf.ignore(200,58); // throw away everything uptil colon character
  inputf >> deltax;
  inputf.ignore(200,32); //throwaway space line before start of populations
  dumpf << "Deltax :" << deltax << endl;
  Poplist poplist(totalnodes,numpops, &connectmat);
  PropagNet propagnet(deltat,deltax,totalnodes,numpops,numconct);
  inputf.ignore(200,32); //throwaway space line before start of populations
  if(argc==1){
//
//      Read in remaining init parameters and initialize the classes
//
    poplist.init(inputf, &propagnet, &connectmat);
    propagnet.init(inputf, &poplist);
  }   
  else{
//
//      Read in remaining restart parameters and initialize the classes
//
    poplist.restart(inputf, &propagnet, &connectmat);
    propagnet.restart(inputf, &poplist);
  }
//
//  Main integration Loop
//
  for(int k=1;k<nsteps+1;k++){
    propagnet.stepQtoP(&poplist, &connectmat);
    poplist.stepPops(deltat);
  }
//
// Dump data for restart
//
  poplist.dump(dumpf);
  propagnet.dump(dumpf);

  return EXIT_SUCCESS;
}

void readglobalparams(ifstream& datainf, ofstream& dumpf, long& totalnodes, int& numpops, int& numconct){
//
//   Read in global parameters
//
  datainf.ignore(200,32); // throwaway first line delimited by ASCII 32
  dumpf << "A dump file for the code 'eegcode'" << endl;
  datainf.ignore(200,58); // throwaway everything before ASCII 58 ":"
  datainf >> totalnodes;
  dumpf << "Nodes per population: " << totalnodes << endl;
  datainf.ignore(200,58); // throwaway everything before ASCII 58 ":"
  datainf >> numpops;
  dumpf << "Number of neural populations: " << numpops << endl;
  datainf.ignore(200,58); // throwaway everything before ASCII 58 ":"
  datainf >> numconct;
  dumpf << "Number of neural connections: " << numconct << endl;
  if(numconct>(numpops*numpops)) cerr << "Number of connections cannot exceed the square of the number of populations";
  datainf.ignore(200,32); // throwaway reaminder of line after Number of connections
}

