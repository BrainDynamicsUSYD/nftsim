/***************************************************************************
                          phiout.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cstdlib>
#include "phiout.h"

Phiout::Phiout(Istrm& inputf, ofstream& outputf, int numconct, long totalnodes){
  int optionnum;
  inputf.ignore(200,58); // throwaway everything up to colon character
  inputf >> numtraces;
  outputf << "Output Data - Number of traces: " << numtraces << endl;
  idoftrace = new int [numtraces];
  nodeoftrace = new long [numtraces];
  long i=0;
  while(i<numtraces){
    int idindex;
    long temp1;
    inputf.ignore(200,58); // throwaway everything up to colon character
    inputf >> idindex;
    if(idindex>numconct || idindex<1){
      std::cerr << "Propagator number " << idindex << " requested for output is invalid" << endl;
      exit(EXIT_FAILURE);
    }
    idoftrace[i]=idindex-1; // '-1' as idoftrace runs [0:n-1] rather than [1:n]
    outputf << "Propagator Number : "<< idindex << " ";
    inputf.ignore(200,58); // throwaway everything up to colon character (Single/All:)
    optionnum=inputf.choose("Single:1 All:2 ",32);
    outputf << "Single/All nodes: ";
    if(1==optionnum){
      outputf << " Single ";
      inputf.ignore(200,58); // throwaway everything up to colon character
      inputf >> temp1;
      if(temp1>totalnodes || temp1<1){
        std::cerr << "Node number " << temp1 << " requested for output is invalid" << endl;
        exit(EXIT_FAILURE);
      }     
      nodeoftrace[i]=temp1-1; //'-1' as nodeoftrace runs [0:n-1] rather than [1:n]
      outputf << "Node Number : "<< temp1 << endl;
    }
    if(2==optionnum){
      outputf << " All nodes" << endl;
      inputf.ignore(200,32); // throw away " nodes (endl)"
      nodeoftrace[i]=0; // Request the first node be outputted
      for(long j=1;j<totalnodes;j++){ // Request the remaining nodes be outputted
        idoftrace[i+j]=idindex-1; // '-1' as idoftrace runs [0:n-1] rather than [1:n]
	nodeoftrace[i+j]=j;
      }
      i+=totalnodes-1; // increment by number of traces now requested (except the one in outer loop)
      if( i>(numtraces-1) ){
        std::cerr << "Not enough traces specified to output all - please increase" << endl;
	exit(EXIT_FAILURE);
      }
    }
    if(1!=optionnum && 2!=optionnum){
      std::cerr << "Error - possibly too many traces specified or invalid output specification" << endl;
      exit(EXIT_FAILURE);
    }
    i++;
  }
}
Phiout::~Phiout(){
  delete [ ] idoftrace;
  delete [ ] nodeoftrace;
}

void Phiout::dump(ofstream& dumpf){
  dumpf << endl;
  dumpf << "Output Data - Number of traces: " << numtraces << endl;
  for(long i=0; i<numtraces;i++){
    dumpf << "Propagator Number : " << (idoftrace[i]+1) << " ";
    dumpf << "Single/All: Single node number:" << (nodeoftrace[i]+1) << endl;
  }
}

void Phiout::output(ofstream& outputf, double **Eta){
  double * start; // a temporary pointer to the start of a Eta array
  for(long i=0; i<numtraces;i++){
    start = Eta[idoftrace[i]];
    outputf << setprecision(14) << *(start+nodeoftrace[i]) << endl; // Add a field with value at Eta point requested
  }
  outputf << endl; // Insert a blank line at the end of the record
}

