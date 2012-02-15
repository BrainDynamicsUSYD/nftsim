/***************************************************************************
                          connectmat.h  -  construct connection matrix so that
                                           classes have reference index for
                                           other classes
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef CONNECTMAT_H
#define CONNECTMAT_H

#include<fstream>
#include<vector>
using std::vector;

#include"istrm.h"

class ConnectMat {
public: 
  ConnectMat( long Nodes, Istrm& inputf, std::ofstream& dumpf, int& nPop, int& nCnt, double deltat, long nSteps, long nSkip, bool restart );
  ~ConnectMat();
  void dump(std::ofstream& dumpf); // Write out raw connection matrix

  // Given a connection index, return the presynaptic population index
  // Useful for finding Q for given connection index
  int getPrePop( int index )const{ return preCnt[index]; };
  // Given a connection index, return the postsynaptic population index
  // Useful for finding dendritic responses for given connection index
  int getPostPop( int index )const{ return postCnt[index]; };
  // Return the number of dendritic responses attached to particular population
  int getDRlength( int index )const{ return nDr[index]; };

private:
  ConnectMat(); // no default constructor
  ConnectMat(ConnectMat& ); // no copy constructor
  vector<int> preCnt; // Array mapping connection number back to Qb arrays
  vector<int> postCnt; // Array mapping connection number to dendritic responses populations
  vector<int> nDr; // Array each element is the number of dendritic responses attached to population number 'index'
  vector< vector<double> > rawCntMat; // Connection matrix, 2D vector
};

#endif
