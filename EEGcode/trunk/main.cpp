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
using std::cerr;
using std::endl;

#include"istrm.h"
#include"connectmat.h"
#include"propagnet.h"
#include"poplist.h"

void readglobalparams(Istrm& datainf, ofstream& dumpf, long& totalnodes, int& numpops, int& numconct);
void writedefault(ofstream& defaultf);

int main(int argc, char* argv[])
{
//
// Open file for dumping data for restart
//
  ofstream dumpf("eegcode.dump",ios::out);
  if( !dumpf ){
    cerr << "Unable to open 'eegcode.dump' for output \n";
    exit(EXIT_FAILURE);
  }
//
// Read in the global simulation parameters
//
  long totalnodes=0;
  int numpops=0;
  int numconct=0;
  cerr << argc << endl;
  if(argc==3){
    ofstream defaultf("eegcode.conf",ios::out);
    if( !defaultf ){
      cerr << "Unable to open 'eegcode.conf' for output of default parameters \n";
      exit(EXIT_FAILURE);
    }
    writedefault(defaultf);
    cerr << "Written 'eegcode.conf' file" << endl;
    defaultf.close();
  }
  Istrm inputf("eegcode.conf"); //open file for reading input data
  if( !inputf ){
    cerr << "Unable to open 'eegcode.conf' for input \n";
    exit(EXIT_FAILURE);
  }
  readglobalparams(inputf, dumpf, totalnodes, numpops, numconct);
  ConnectMat connectmat(numpops,numconct);
  connectmat.init(inputf);
  connectmat.dump(dumpf);
//
// Construct the classes
//
  int nsteps;
  inputf.validate("integration steps",58); 
  inputf >> nsteps;
  dumpf << "Number of integration steps:" << nsteps << " ";
  float deltat, deltax;
  inputf.validate("Deltat",58);
  inputf >> deltat;
  dumpf << "Deltat :" << deltat << " ";
  inputf.validate("Deltax",58);
  inputf >> deltax;
  dumpf << "Deltax:" << deltax << endl;
  inputf.ignore(200,32); //throwaway space line before start of populations
  Poplist poplist(totalnodes,numpops, &connectmat);
  PropagNet propagnet(deltat,deltax,totalnodes,numpops,numconct,inputf,dumpf);
  inputf.ignore(200,32); //throwaway space line before start of populations
  if(argc==1 || argc==3){
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
//  Initialize the output routine
//
//
// Open file for outputting data 
//
  ofstream outputf("eegcode.output",ios::out);
  if( !outputf ){
    cerr << "Unable to open 'eegcode.output' for output \n";
    exit(EXIT_FAILURE);
  }
  outputf << "Deltat :" << deltat << endl;
  outputf << "Number of integration steps :" << nsteps << endl;
  propagnet.initoutput(inputf,outputf);
//
//  Main integration Loop
//
  for(int k=1;k<nsteps+1;k++){
    propagnet.stepQtoP(&poplist, &connectmat);
    poplist.stepPops(deltat);
    propagnet.output(outputf);
  }
//
// Dump data for restart
//
  poplist.dump(dumpf);
  propagnet.dump(dumpf);

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
    cerr << "Number of connections cannot exceed the square of the number of populations" << endl;
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
  defaultf << "1			     :1  :2  :3  :4  :0" << endl;
  defaultf << "2			     :5  :6  :0  :0  :0" << endl;
  defaultf << "3			     :0  :0  :0  :7  :0" << endl;
  defaultf << "4			     :8  :9  :10 :0  :0" << endl;
  defaultf << "5			     :0  :0  :0  :11 :0 " << endl;
  defaultf << "Number of integration steps: 100000 Deltat: 0.0001 Deltax: 0.0035" << endl;
  defaultf << "Q delay depths: 420 : 0 : 0 : 420 :0" << endl;
  defaultf << "" << endl;
  defaultf << "Population 1 - Excitory neurons" << endl;
  defaultf << " Steady State Q: 8.87145" << endl;
  defaultf << " Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0" << endl;
  defaultf << " Number of Dendritic responses: 3" << endl;
  defaultf << "  Dendritic response from population 1 V initial: 0.0106457 alpha: 75.0 beta: 285.0 " << endl;
  defaultf << "  Dendritic response from population 2 V initial:-0.0159686 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "  Dendritic response from population 4 V initial: 0.0057734 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "" << endl;
  defaultf << "Population 2 - Inhibitory neurons" << endl;
  defaultf << "  Steady State Q: 8.87145" << endl;
  defaultf << "  Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0" << endl;
  defaultf << "  Number of Dendritic responses: 3" << endl;
  defaultf << "   Dendritic response from population 1 V initial: 0.0106457 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 2 V initial:-0.0159686 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 4 V initial: 0.0057734 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "" << endl;
  defaultf << "Population 3 - Reticular nucleus neurons" << endl;
  defaultf << "  Steady State Q: 15.7298" << endl;
  defaultf << "  Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0" << endl;
  defaultf << "  Number of Dendritic responses: 2" << endl;
  defaultf << "   Dendritic response from population 1 V initial: 0.00177429 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 4 V initial: 0.000962233 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "" << endl;
  defaultf << "Population 4 - Relay nuclei neurons" << endl;
  defaultf << "  Steady State Q: 4.81117" << endl;
  defaultf << "  Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0" << endl;
  defaultf << "  Number of Dendritic responses: 3" << endl;
  defaultf << "   Dendritic response from population 1 V initial: 0.0106457 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 3 V initial:-0.0125839 alpha: 75.0 beta: 285.0" << endl;
  defaultf << "   Dendritic response from population 5 V initial: 0.00	  alpha: 75.0 beta: 285.0" << endl;
  defaultf << "" << endl;
  defaultf << "Population 5 - Stimulus neurons" << endl;
  defaultf << "  Steady State Q: 0.0" << endl;
  defaultf << "  Stimulus mode: 4 Time to start of stimulus: 0.0002 Amplitude: 20" << endl;
  defaultf << "Propagation data" << endl;
  defaultf << "Wave equation 1  - Steady State Phi: 8.87145 Tauab: 0	Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Wave equation 2  - Steady State Phi: 8.87145 Tauab: 0	Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Wave equation 3  - Steady State Phi: 8.87145 Tauab: 420 Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Wave equation 4  - Steady State Phi: 8.87145 Tauab: 420 Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Wave equation 5  - Steady State Phi: 8.87145 Tauab: 0	Effective range: 0.0001 gamma: 80000.0" << endl;
  defaultf << "Wave equation 6  - Steady State Phi: 8.87145 Tauab: 0	Effective range: 0.0001 gamma: 80000.0" << endl;
  defaultf << "Wave equation 7  - Steady State Phi: 15.7298 Tauab: 0	Effective range: 0.0001 gamma: 80000.0" << endl;
  defaultf << "Wave equation 8  - Steady State Phi: 4.81117 Tauab: 420 Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Wave equation 9  - Steady State Phi: 4.81117 Tauab: 420 Effective range: 0.08 gamma: 100.0" << endl;
  defaultf << "Wave equation 10 - Steady State Phi: 4.81117 Tauab: 0	Effective range: 0.0001 gamma: 80000.0" << endl;
  defaultf << "Wave equation 11 - Steady State Phi: 0.0     Tauab: 0	Effective range: 0.0001 gamma: 80000.0" << endl;
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
  defaultf << "Wave Equation Number :0 node number :1" << endl;
  defaultf << "Wave Equation Number :4 node number :1" << endl;
  defaultf << "Wave Equation Number :6 node number :1" << endl;
  defaultf << "Wave Equation Number :7 node number :1" << endl;
}
