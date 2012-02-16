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
#include<cmath>
#include"connectmat.h"
#include"ostrm.h"
#include"poplist.h"
#include"propagnet.h"
#include"configf.h"
using std::endl;

ConnectMat::ConnectMat(void)
{
}

ConnectMat::~ConnectMat()
{
}

void ConnectMat::init( Configf& inputf )
{
  // Parse in global parameters from conf file
  // Anything before Nodes per population is ignored as comment
  inputf.Param("Integration steps",steps);
  if( !inputf.Optional("Skip steps",skip) ) skip = 0;
  inputf.Param("Deltat",deltat);
  //dumpf << " Deltat: " << deltat << endl;
  int nodes; inputf.Param("Nodes",nodes);
  int longside;
  if( inputf.Optional("Longside",longside) ) {
    if (nodes%longside!= 0) {
      std::cerr << "To define a rectangular grid nodes: " << nodes <<endl
        << "divided by Longside: " << longside << endl
        << "must have no remainder" << endl;
      exit(EXIT_FAILURE); 
    }
  }
  else
    longside = sqrt(nodes);
  //dumpf << "Dump file for NeuroField" << endl
    //<< "Nodes: " << Nodes << endl;

  // glutamate dynamics is loaded by Couplinglist
  inputf.Next("Lambda"); inputf.Next("tGlu");

  inputf.Next("Connection matrix");
  // Read the number of populations
  inputf.Next("From");
  // Expect format "From: 1 2 3 4", read the 
  vector<double> pop = inputf.Numbers();
  int npop = pop.size();

  rawCntMat.resize(npop);
  nDr.resize(npop,0);

  for( int i=0; i<npop; i++ ) {
    inputf.ignore(':'); // ignore "To ?:"
    rawCntMat[i] = inputf.Numbers();
    if( rawCntMat[i].size() != uint(npop) ) {
      std::cerr << "The connection matrix is not configured correctly."
        << endl;
      exit(EXIT_FAILURE);
    }
  }

  // stores the presynaptic population index for each connection index
  for( int j=0; j<npop; j++ )
    for( int i=0; i<npop; i++ )
      if( rawCntMat[i][j] )
        preCnt.push_back(j);

  // stores the postsynaptic population index for each connection index
  for( int j=0; j<npop; j++ )
    for( int i=0; i<npop; i++ )
      if( rawCntMat[i][j] )
        postCnt.push_back(i);

  // number of dendritic response for each population
  for( int i=0; i<npop; i++ )
    for( unsigned int j=0; j<postCnt.size(); j++ )
      if( postCnt[j] == i )
        nDr[i]++;

  int ncnt = postCnt.size(); // == preCnt.size()

  // Construct the classes
  Poplist poplist(nodes,npop,*this);
  //PropagNet propagnet(deltat,nodes,npop,ncnt,inputf);
  //inputf>>poplist; inputf>>propagnet;

  Ostrm ostrm(nodes); inputf.Param("Outputs",ostrm);
  for( int i=0; i<3; i++ ) {
    if( inputf.Next("Propag") ) {
      vector<double> temp = inputf.Numbers();
      for( uint i=0; i<temp.size(); i++ ) {
        if( i >= uint(ncnt) ) {
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
}

void ConnectMat::solve(void)
{
  for( int i=0; i<steps; i++ ) {
    step();
    //if( skip==0 )
      //ostrm.step();
    //else
      //skip--;
  }
}

void ConnectMat::restart( Restartf& restartf )
{
}


void ConnectMat::step(void)
{
  //propagnet.stepQtoP(poplist,connectmat);
  //poplist.stepPops(deltat);
}

void ConnectMat::dump(std::ofstream& dumpf) const
{
  dumpf << "Connection matrix:" << endl
    << "From:";
  for( unsigned int i=0; i<nDr.size(); i++ )
    dumpf << " " << i;
  for( unsigned int i=0; i<nDr.size(); i++ ) {
    dumpf << endl << "To " << i+1 << ":";
    for( unsigned int j=0; j<nDr.size(); j++ )
      dumpf << " " << rawCntMat[i][j];
  }
  dumpf << endl << endl;
  //dumpf<<poplist;
  //dumpf<<propagnet;
}
