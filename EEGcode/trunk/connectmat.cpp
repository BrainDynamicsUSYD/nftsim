/***************************************************************************
                          connectmat.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cstdlib>
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

void ConnectMat::init(Istrm& inputf){
  //
  // This part reads in the raw connection matrix
  //
  inputf.ignore(200,32); // throwaway title line of connection matrix
  for(int i=0;i<(nump*nump);i++){
    inputf.ignore(200,58); // throwaway everything upto colon character
    int readval;
    inputf >> readval; // read in a connection number 0 or connection number
    rawcntmat[i]=readval;
  }
  // 
  // To simplify parsing of the raw matrix, the matrix is transposed before parsing
  // The matrix will be returned to correct form at the end of the parsing
  //
  int swap;
  for(int i=0;i<nump;i++){
    for(int j=(i+1);j<nump;j++){
    swap=rawcntmat[i*nump+j];
    rawcntmat[i*nump+j]=rawcntmat[j*nump+i];
    rawcntmat[j*nump+i]=swap;
    }
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
      else {
	cerr << counter <<"Too many connections in connection matrix relative to stated number of connections" <<endl;
        exit(EXIT_FAILURE);
      }
    }
  }
  if(counter!=numconnect){
    cerr << "Too few connections in connection matrix relative to stated number of connections" <<endl;
    exit(EXIT_FAILURE);
  }
  //
  // This part takes the raw connection matrix and generates a drphiconnect index table
  // the drphiconnect index table allows a dendritic response list with index drphiconnect[i]
  // to REVERSE lookup the index i of the connected phi population
  counter=0;
  for(int i=0;i<(nump*nump);i++){
    if(rawcntmat[i]!=0){
      if(counter<numconnect){
        drphiconnect[counter]=i%nump;
	counter++;
      }	
      else {
	cerr << counter <<"Too many connections in connection matrix relative to stated number of connections" <<endl;
        exit(EXIT_FAILURE);
      }
    }
  }
  if(counter!=numconnect){
    cerr << "Too few connections in connection matrix relative to stated number of connections" <<endl;
    exit(EXIT_FAILURE);
  }
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
  //
  // Return the raw connection matrix back to its correct form now that parsing is finished
  //
  for(int i=0;i<nump;i++){
    for(int j=(i+1);j<nump;j++){
    swap=rawcntmat[i*nump+j];
    rawcntmat[i*nump+j]=rawcntmat[j*nump+i];
    rawcntmat[j*nump+i]=swap;
    }
  }
}

void ConnectMat::dump(ofstream& dumpf){
  dumpf << "Connection Matrix " << endl;
  for (int i=0; i<(nump*nump); i++){
    dumpf << ": " << rawcntmat[i] << " ";
  }
  dumpf << endl; // Append an endl to Connection matrix data
}
