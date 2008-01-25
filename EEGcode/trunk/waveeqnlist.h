/***************************************************************************
                          waveeqnlist.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef WAVEEQNLIST_H
#define WAVEEQNLIST_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"waveeqn.h"
#include"qhistorylist.h"
#include"connectmat.h"

class WaveEqnlist {
public: 
  WaveEqnlist(int numconnects, long gridsize, float deltat, float deltax);
  ~WaveEqnlist();
  void init(ifstream& inputf);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
  void stepWaveEqns(float **Eta, Qhistorylist * pqhistorylist, ConnectMat * pconnectmat); // Transform Q to Eta via stepping forward multiple wave equations

private:
  WaveEqn * firstlink; // link to first wave equation in the wave equation linked list
  WaveEqn * getwaveeq(int index); // method to obtain pointer to "i"th wave equation
  const int numwaveeq; //number of wave equation objects in the list
};

#endif
