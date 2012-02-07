/***************************************************************************
                          connectmat.cpp  -  construction of connection matrix
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
#include<vector>
using std::vector;
#include"connectmat.h"
using std::endl;

ConnectMat::ConnectMat(Istrm& inputf, int& nPop, int& nCnt )
{
  // Read the number of populations
  inputf.ignore(200,':');
  // Expect format "From: 1 2 3 4", read the 
  vector<double> pop = inputf.Numbers();
  nPop = pop.size();

  rawCntMat.resize(nPop);
  nDr.resize(nPop,0);

  for( int i=0; i<nPop; i++ ) {
    inputf.ignore(200,':'); // ignore "To ?:"
    rawCntMat[i] = inputf.Numbers();
    if( rawCntMat[i].size() != uint(nPop) ) {
      std::cerr << "The connection matrix is not configured correctly."
        << endl;
      exit(EXIT_FAILURE);
    }
  }

  // stores the presynaptic population index for each connection index
  for( int i=0; i<nPop; i++ )
    for( int j=0; j<nPop; j++ )
      if( rawCntMat[i][j] )
        preCnt.push_back(j);

  // stores the postsynaptic population index for each connection index
  for( int i=0; i<nPop; i++ )
    for( int j=0; j<nPop; j++ )
      if( rawCntMat[i][j] )
        postCnt.push_back(i);

  // number of dendritic response for each population
  for( int i=0; i<nPop; i++ )
    for( unsigned int j=0; j<postCnt.size(); j++ )
      if( postCnt[j] == i )
        nDr[i]++;

  nCnt = postCnt.size(); // == preCnt.size()
}

ConnectMat::~ConnectMat(){
}

void ConnectMat::dump(std::ofstream& dumpf){
  dumpf << "Connection Matrix:" << endl
    << "From:";
  for( unsigned int i=0; i<nDr.size(); i++ )
    dumpf << " " << i;
  for( unsigned int i=0; i<nDr.size(); i++ ) {
    dumpf << endl << "To " << i+1 << ":";
    for( unsigned int j=0; j<nDr.size(); j++ )
      dumpf << " " << rawCntMat[i][j];
  }
  dumpf << endl << endl;
}
