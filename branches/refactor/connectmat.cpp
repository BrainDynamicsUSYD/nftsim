/***************************************************************************
                          connectmat.cpp  -  construction of connection matrix
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<fstream>
#include<iostream>
#include<vector>
using std::vector;
#include<sstream>
using std::stringstream;
#include"connectmat.h"
#include"ostrm.h"
#include"poplist.h"
#include"propagnet.h"
using std::endl;

ConnectMat::ConnectMat(long Nodes, Istrm& inputf, std::ofstream& dumpf, int& nPop, int& nCnt, double deltat, long nSteps, long nSkip, bool restart )
{
  // Read the number of populations
  inputf.ignore(':');
  // Expect format "From: 1 2 3 4", read the 
  vector<double> pop = inputf.Numbers();
  nPop = pop.size();

  rawCntMat.resize(nPop);
  nDr.resize(nPop,0);

  for( int i=0; i<nPop; i++ ) {
    inputf.ignore(':'); // ignore "To ?:"
    rawCntMat[i] = inputf.Numbers();
    if( rawCntMat[i].size() != uint(nPop) ) {
      std::cerr << "The connection matrix is not configured correctly."
        << endl;
      exit(EXIT_FAILURE);
    }
  }

  // stores the presynaptic population index for each connection index
  for( int j=0; j<nPop; j++ )
    for( int i=0; i<nPop; i++ )
      if( rawCntMat[i][j] )
        preCnt.push_back(j);

  // stores the postsynaptic population index for each connection index
  for( int j=0; j<nPop; j++ )
    for( int i=0; i<nPop; i++ )
      if( rawCntMat[i][j] )
        postCnt.push_back(i);

  // number of dendritic response for each population
  for( int i=0; i<nPop; i++ )
    for( unsigned int j=0; j<postCnt.size(); j++ )
      if( postCnt[j] == i )
        nDr[i]++;

  nCnt = postCnt.size(); // == preCnt.size()

  // Construct the classes
  Poplist poplist(Nodes,nPop,*this);
  PropagNet propagnet(deltat,Nodes,nPop,nCnt,inputf,dumpf);
  if(restart) { // restart mode
      poplist.restart(inputf,propagnet,*this);
      propagnet.restart(inputf,poplist,*this);
  }   
  else { // normal mode
    poplist.init(inputf,propagnet,*this);
    propagnet.init(inputf,poplist,*this);
  }

  Ostrm ostrm(Nodes); inputf.Param("Outputs",ostrm);
  for( int i=0; i<3; i++ ) {
    if( inputf.Next("Propag") ) {
      vector<double> temp = inputf.Numbers();
      for( uint i=0; i<temp.size(); i++ ) {
        if( i >= uint(nCnt) ) {
          std::cerr<<"Error: trying to output propagator "<<i
            <<", which is outside the bound of all propagators."<<endl;
          exit(EXIT_FAILURE);
        }
        stringstream ss; ss<<"neurofield.propag."<<temp[i];
        //ostrm.Register( ss.str(), propagnet.get(temp[i]-1).Eta );
      }
    }
    else if( inputf.Next("Population") ) {
    }
    else if( inputf.Next("Couple") ) {
    }
  }

  // Open file for outputting data 
  /*int ioutarg = 0;
  if( argc>2 )
    for(int i=1;i<(argc-1);i++)
      if(strcmp(argv[i],"-o")==0)
        ioutarg = i + 1;
  ofstream outputf( (ioutarg?argv[ioutarg]:"neurofield.output"));
  if( !outputf ) {
    std::cerr << "Unable to open "
      << (ioutarg?argv[ioutarg]:"neurofield.output") << " for output.\n";
    exit(EXIT_FAILURE);
  }
  outputf.precision(14);*/

  // Initialize the output routine
  //if(nSkip!=0) outputf << "Skippoints: " << nSkip << " ";
  //outputf << "Deltat: " << deltat << endl;
  //outputf << "Number of integration steps:" << nSteps << endl;
  //propagnet.initoutput(inputf,outputf,nCnt,Nodes);

  //  Main integration Loop
  long skip=nSkip;
  for(int k=1;k<nSteps+1;k++){
    propagnet.stepQtoP(poplist,*this);
    poplist.stepPops(deltat);
    if(0==skip){
      ostrm.Step();
      //propagnet.output(outputf);
      skip=nSkip;
    } else {
      skip--;
    }
  }

  // Dump data for restart
  poplist.dump(dumpf);
  propagnet.dump(dumpf);
  propagnet.dumpoutput(dumpf);
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
