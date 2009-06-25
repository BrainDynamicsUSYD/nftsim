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

#include"istrm.h"
#include"connectmat.h"
#include"propagnet.h"
#include"poplist.h"

void readglobalparams(Istrm& datainf, ofstream& dumpf, long& totalnodes, int& numpops, int& numconct);
void writedefault(ofstream& defaultf);

int main(int argc, char* argv[])
{
//
// Open file for dumping data for restart -default is eegcode.dump
//
  int idumparg=0; // Index No. of dump file name in argv
  if(argc>2){
    for(int i=1;i<(argc-1);i++){
      if(strcmp(argv[i],"-d")==0){
        idumparg=i+1;
      }
    }
  }
  ofstream dumpf(idumparg?argv[idumparg]:"eegcode.dump",ios::out);
  if( !dumpf ){
    std::cerr << "Unable to open "<< (idumparg?argv[idumparg]:"eegcode.dump") << " for output \n";
    exit(EXIT_FAILURE);
  }
  dumpf << setprecision(14);
//
// Read in the global simulation parameters
//
  long totalnodes=0;
  int numpops=0;
  int numconct=0;
  if(argc==3 && (strcmp(argv[1],"write")==0)
             && (strcmp(argv[2],"defaults")==0)){
    ofstream defaultf("eegcode.conf",ios::out);
    if( !defaultf ){
      std::cerr << "Unable to open 'eegcode.conf' for output of default parameters \n";
      exit(EXIT_FAILURE);
    }
    writedefault(defaultf);
    std::cerr << "Written 'eegcode.conf' file" << endl;
    defaultf.close();
  }
//
// Open conf file - default is eegcode.conf
//
  int inamearg=0;
  if(argc>2){
    for(int i=1;i<(argc-1);i++){
      if(strcmp(argv[i],"-i")==0){
        inamearg=i+1;
      }
    }
  }
  Istrm inputf(inamearg?argv[inamearg]:"eegcode.conf"); //open file for reading input data
  if( !inputf ){
    std::cerr << "Unable to open "<< (inamearg?argv[inamearg]:"eegcode.conf") << " for input \n";
    exit(EXIT_FAILURE);
  }
  readglobalparams(inputf, dumpf, totalnodes, numpops, numconct);
  ConnectMat connectmat(numpops,numconct,inputf);
  connectmat.dump(dumpf);
//
// Construct the classes
//
  int nsteps;
  inputf.validate("integration steps",58); 
  inputf >> nsteps;
  dumpf << "Number of integration steps:" << nsteps << " ";
  double deltat;
  long skippts;
  int optionnum;
  optionnum=inputf.choose("Deltat:1 Skippoints:2 ",58);
  if(1==optionnum){
    inputf >> deltat;
    skippts=0;
    dumpf << "Deltat:" << deltat << " ";
  } else {
    inputf >> skippts;
    inputf.validate("Deltat",58);
    inputf >> deltat;
    dumpf << "Skippoints:"<< skippts << " " << "Deltat:" << deltat << " ";
  }
  inputf.ignore(200,32); //throwaway space line before start of populations
  Poplist poplist(totalnodes,numpops, &connectmat);
  PropagNet propagnet(deltat,totalnodes,numpops,numconct,inputf,dumpf);
  inputf.ignore(200,32); //throwaway space line before start of populations
  if( !(argc>=2 && (strcmp(argv[1],"restart")==0)) ){
//
//      Read in remaining init parameters and initialize the classes
//
    poplist.init(inputf, &propagnet, &connectmat);
    propagnet.init(inputf, &poplist, &connectmat);
  }   
  else{
//
//      Read in remaining restart parameters and initialize the classes
//
    poplist.restart(inputf, &propagnet, &connectmat);
    propagnet.restart(inputf, &poplist);
  }
//
//  Initialize the output routine
//
//
// Open file for outputting data 
//
  int ioutarg=0;
  if(argc>2){
    for(int i=1;i<(argc-1);i++){
      if(strcmp(argv[i],"-o")==0){
        ioutarg=i+1;
      }
    }
  }
  ofstream outputf(ioutarg?argv[ioutarg]:"eegcode.output",ios::out);
  if( !outputf ){
    std::cerr << "Unable to open "<< (ioutarg?argv[ioutarg]:"eegcode.output") << " for output \n";
    exit(EXIT_FAILURE);
  }
  if(skippts!=0) outputf << "Skippoints: " << skippts << " ";
  outputf << "Deltat: " << deltat << endl;
  outputf << "Number of integration steps:" << nsteps << endl;
  propagnet.initoutput(inputf,outputf,numconct,totalnodes);
//
//  Main integration Loop
//
  long skip=skippts;
  for(int k=1;k<nsteps+1;k++){
    propagnet.stepQtoP(&poplist, &connectmat);
    poplist.stepPops(deltat);
    if(0==skip){
      propagnet.output(outputf);
      skip=skippts;
    } else {
      skip--;
    }
  }
//
// Dump data for restart
//
  poplist.dump(dumpf);
  propagnet.dump(dumpf);
  propagnet.dumpoutput(dumpf);

  return EXIT_SUCCESS;
}

void readglobalparams(Istrm& datainf, ofstream& dumpf, long& totalnodes, int& numpops, int& numconct){
//
//   Read in global parameters
//
  datainf.ignore(200,32); // throwaway first line delimited by ASCII 32
  dumpf << "A dump file for the code 'eegcode'" << endl;
  datainf.validate("Nodes per population",58);
  datainf >> totalnodes;
  dumpf << "Nodes per population: " << totalnodes << endl;
  datainf.validate("Number of neural populations",58);
  datainf >> numpops;
  dumpf << "Number of neural populations: " << numpops << endl;
  datainf.validate("Number of neural connections",58);
  datainf >> numconct;
  dumpf << "Number of neural connections: " << numconct << endl;
  if(numconct>(numpops*numpops)){
    std::cerr << "Number of connections cannot exceed the square of the number of populations" << endl;
    exit(EXIT_FAILURE);
  }
  datainf.ignore(200,32); // throwaway reaminder of line after Number of connections
}

void writedefault(ofstream& defaultf){
//
// Write a default series of parameters to eegcode.conf
//
  defaultf << "A Configuration file for the code 'eegcode'" << endl;
  defaultf << "Nodes per population: 2500" << endl;
  defaultf << "Number of neural populations: 5" << endl;
  defaultf << "Number of neural connections: 11" << endl;
  defaultf << "" << endl;
  defaultf << "Population connection matrix  1   2   3   4   5" << endl;
  defaultf << "1			     :1  :5  :0  :8  :0" << endl;
  defaultf << "2			     :2  :6  :0  :9  :0" << endl;
  defaultf << "3			     :3  :0  :0  :10 :0" << endl;
  defaultf << "4			     :4  :0  :7  :0  :11" << endl;
  defaultf << "5			     :0  :0  :0  :0  :0 " << endl;
  defaultf << "Number of integration steps: 100000 Deltat: 0.0001 " << endl;
  defaultf << "Q delay depths: 420 : 0 : 0 : 420 :0" << endl;
  defaultf << "Propagator types 1: Waveeqn 2: Waveeqn 3: Waveeqn 4: Waveeqn 5: Mapping $" << endl;
  defaultf << "6: Mapping 7: Mapping 8: Mapping 9: Mapping 10: Mapping 11: Mapping $" << endl;
  defaultf << "" << endl;
  defaultf << "Coupling types 1: Simple 2: Simple 3: Simple 4: Simple 5: Simple 6: Simple $" << endl;
  defaultf << "7: Simple 8: Simple 9: Simple 10: Simple 11: Simple $" << endl;
  defaultf << "" << endl;
  defaultf << "Population 1 - Excitory neurons" << endl;
  defaultf << " Initial Q: 8.87145" << endl;
  defaultf << " Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0" << endl;
  defaultf << " Number of Dendritic responses: 3" << endl;
  defaultf << "  Dendritic response from population 1 V initial: 0.0106457 alpha: 75.0 beta: 285.0 " << endl;
  defaultf << "  Dendritic response from population 2 V initial:-0.0159686 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "  Dendritic response from population 4 V initial: 0.0057734 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "" << endl;
  defaultf << "Population 2 - Inhibitory neurons" << endl;
  defaultf << "  Initial Q: 8.87145" << endl;
  defaultf << "  Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0" << endl;
  defaultf << "  Number of Dendritic responses: 3" << endl;
  defaultf << "   Dendritic response from population 1 V initial: 0.0106457 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 2 V initial:-0.0159686 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 4 V initial: 0.0057734 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "" << endl;
  defaultf << "Population 3 - Reticular nucleus neurons" << endl;
  defaultf << "  Initial Q: 15.7298" << endl;
  defaultf << "  Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0" << endl;
  defaultf << "  Number of Dendritic responses: 2" << endl;
  defaultf << "   Dendritic response from population 1 V initial: 0.00177429 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 4 V initial: 0.000962233 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "" << endl;
  defaultf << "Population 4 - Relay nuclei neurons" << endl;
  defaultf << "  Initial Q: 4.81117" << endl;
  defaultf << "  Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0" << endl;
  defaultf << "  Number of Dendritic responses: 3" << endl;
  defaultf << "   Dendritic response from population 1 V initial: 0.0106457 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 3 V initial:-0.0125839 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 5 V initial: 0.00	  alpha: 75.0 beta: 285.0" << endl;
  defaultf << "" << endl;
  defaultf << "Population 5 - Stimulus neurons" << endl;
  defaultf << "  Initial Q: 0.0" << endl;
  defaultf << "  Stimulus mode: 2 Time to start of stimulus: 0.0002 Amplitude: 2 Mean: 5" << endl;
  defaultf << "Propagation data" << endl;
  defaultf << "Propagator 1  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 0   Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Propagator 2  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 0   Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Propagator 3  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 420 Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Propagator 4  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 420 Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Propagator 5  - Tauab: 0   " << endl;
  defaultf << "Propagator 6  - Tauab: 0   " << endl;
  defaultf << "Propagator 7  - Tauab: 0   " << endl;
  defaultf << "Propagator 8  - Tauab: 420" << endl;
  defaultf << "Propagator 9  - Tauab: 420" << endl;
  defaultf << "Propagator 10 - Tauab: 0   " << endl;
  defaultf << "Propagator 11 - Tauab: 0   " << endl;
  defaultf << "Coupling data 1  - Nu: 0.0012" << endl;
  defaultf << "Coupling data 2  - Nu: 0.0012" << endl;
  defaultf << "Coupling data 3  - Nu: 0.0002" << endl;
  defaultf << "Coupling data 4  - Nu: 0.0012" << endl;
  defaultf << "Coupling data 5  - Nu:-0.0018" << endl;
  defaultf << "Coupling data 6  - Nu:-0.0018" << endl;
  defaultf << "Coupling data 7  - Nu:-0.0008" << endl;
  defaultf << "Coupling data 8  - Nu: 0.0012" << endl;
  defaultf << "Coupling data 9  - Nu: 0.0012" << endl;
  defaultf << "Coupling data 10 - Nu: 0.0002" << endl;
  defaultf << "Coupling data 11 - Nu: 0.00015" << endl;
  defaultf << "" << endl;
  defaultf << "Output Data - Number of traces : 4 " << endl;
  defaultf << "Propagator Number :1 Single/All: Single node number :1" << endl;
  defaultf << "Propagator Number :5 Single/All: Single node number :1" << endl;
  defaultf << "Propagator Number :7 Single/All: Single node number :1" << endl;
  defaultf << "Propagator Number :8 Single/All: Single node number :1" << endl;
}
