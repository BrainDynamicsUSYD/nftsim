/***************************************************************************
                          waveeqnlist.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef WAVEEQNLIST_H
#define WAVEEQNLIST_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"waveeqn.h"
#include"qhistorylist.h"
#include"connectmat.h"

class WaveEqnlist {
public: 
  WaveEqnlist(int numconnects, long gridsize, float deltat, float deltax);
  ~WaveEqnlist();
  void init(Istrm& inputf);
  void dump(ofstream& dumpf);
  void restart(Istrm& restartf);
  void stepWaveEqns(float **Eta, Qhistorylist * pqhistorylist, ConnectMat * pconnectmat); // Transform Q to Eta via stepping forward multiple wave equations

private:
  WaveEqn ** waveeqnarray; // Array of pointers to wave equation objects
  WaveEqn * getwaveeq(int index); // method to obtain pointer to "i"th wave equation
  const int numwaveeq; //number of wave equation objects in the list
};

#endif
