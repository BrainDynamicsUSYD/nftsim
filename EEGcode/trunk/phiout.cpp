/***************************************************************************
                          phioout.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "phiout.h"

Phiout::Phiout(ifstream& inputf, ofstream& outputf){
  inputf.ignore(200,58); // throwaway everything up to colon character
  inputf >> numtraces;
  outputf << "Output Data - Number of traces: " << numtraces << endl;
  idoftrace = new int [numtraces];
  nodeoftrace = new long [numtraces];
  for(long i=0;i<numtraces;i++){
    int temp;
    long temp1;
    inputf.ignore(200,58); // throwaway everything up to colon character
    inputf >> temp;
    idoftrace[i]=temp;
    outputf << "Wave Equation Number : "<< temp << " ";
    inputf.ignore(200,58); // throwaway everything up to colon character
    inputf >> temp1;
    nodeoftrace[i]=temp1;
    outputf << "Node Number : "<< temp1 << endl;
  }
}
Phiout::~Phiout(){
  delete [ ] idoftrace;
  delete [ ] nodeoftrace;
}

void Phiout::output(ofstream& outputf, float **Eta){
  float * start; // a temporary pointer to the start of a Eta array
  for(long i=0; i<numtraces;i++){
    start = Eta[idoftrace[i]];
    outputf << setprecision(6) << *(start+nodeoftrace[i]) << endl; // Add a field with value at Eta point requested
  }
  outputf << endl; // Insert a blank line at the end of the record
}

