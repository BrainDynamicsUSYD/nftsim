/***************************************************************************
                          connectmat.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include"connectmat.h"

ConnectMat::ConnectMat(int numpops,int numconct):numconnect(numconct),nump(numpops){
  rawcntmat = new int[nump*nump];
  qphiconnect = new int[numconct];
  drphiconnect = new int[numconct];
  drlength = new int[nump];
}

ConnectMat::~ConnectMat(){
  delete [ ] rawcntmat;
  delete [ ] qphiconnect;
  delete [ ] drphiconnect;
  delete [ ] drlength;
}

void ConnectMat::init(ifstream& inputf){
  //
  // This part reads in the raw connection matrix
  //
  inputf.ignore(200,32); // throwaway title line of connection matrix
  for(int i=0;i<(nump*nump);i++){
    inputf.ignore(200,58); // throwaway everything upto colon character
    int readval;
    inputf >> readval; // read in a connection number 0 or 1
    rawcntmat[i]=readval;
  }
  //
  // This part takes the raw connection matrix and generates a qphiconnect index table
  // the qphiconnect index table allows a phi population with index "i" to obtain qphiconnect[i]
  // which is the index of the connected Q population
  //
  int counter=0;
  for(int i=0;i<(nump*nump);i++){
    if(rawcntmat[i]!=0){
      if(counter<numconnect){
        qphiconnect[counter]=i/nump;
	counter++;
      }	
      else cerr << counter << "Too many connections in connection matrix relative to stated number of connections" <<endl;
    }
  }
  if(counter!=numconnect) cerr << "Too few connections in connection matrix relative to stated number of connections" <<endl;
  //
  // This part takes the raw connection matrix and generates a drphiconnect index table
  // the drphiconnect index table allows a dendritic response list with index drphiconnect[i]
  // to REVERSE lookup the index i of the connected phi population
  counter=0;
  for(int i=0;i<nump;i++){
    for(int j=0;j<nump;j++){
      if(rawcntmat[i+j*nump]!=0){
        if(counter<numconnect){
          drphiconnect[counter]=i;
	  counter++;
	}
        else cerr << counter << numconnect << "Too many connections in connection matrix relative to stated number of connections" <<endl;
      }
    }
  }
  if(counter!=numconnect) cerr << "Too few connections in connection matrix relative to stated number of connections" <<endl;
  //
  // This part builds drlength array
  for(int i=0;i<nump;i++){
    counter=0;
    for(int j=0;j<numconnect;j++){
     if(drphiconnect[j]==i)
       counter++;
    }
  drlength[i]=counter;
  }
}

void ConnectMat::dump(ofstream& dumpf){
  dumpf << "Connection Matrix " << endl;
  for (int i=0; i<(nump*nump); i++){
    dumpf << ": " << rawcntmat[i] << " ";
  }
  dumpf << endl; // Append an endl to Connection matrix data
}
