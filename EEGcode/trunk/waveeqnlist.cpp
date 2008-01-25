/***************************************************************************
                          waveeqnlist.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "waveeqnlist.h"

//
// Constructor for WaveEqnlist creates a linked list of wave equation objects
//

WaveEqnlist::WaveEqnlist(int numconnects, long gridsize, float deltat, float deltax):numwaveeq(numconnects){
  waveeqnarray = new WaveEqn *[numwaveeq];
  for(int i=0;i<numwaveeq;i++){
    waveeqnarray[i] = new WaveEqn(gridsize, deltat, deltax);
  }
}

//
// Destructor deletes each wave equation object and then array which holds them
//

WaveEqnlist::~WaveEqnlist(){
  for(int i=0;i<numwaveeq; i++)
    delete getwaveeq(i);
  delete [ ] waveeqnarray;
}

//
// get method returns a pointer to the "index"th wave equation object in the list
//

WaveEqn * WaveEqnlist::getwaveeq(int index){
  return waveeqnarray[index];
}

//
//  init method initializes wave equation object in turn
//

void WaveEqnlist::init(ifstream& inputf){
  for(int i=0; i<numwaveeq; i++)
    getwaveeq(i)->init(inputf);
}

//
//  dump method dumps wave equation objects in turn for restart
//
void WaveEqnlist::dump(ofstream& dumpf){
  for(int i=0; i<numwaveeq; i++){
    dumpf << "Wave Eqn "<< (i+1) << " ";
    getwaveeq(i)->dump(dumpf);
    }
}

void WaveEqnlist::restart(ifstream& restartf){
  for(int i=0; i<numwaveeq; i++)
    getwaveeq(i)->restart(restartf);
}

//
// stepWaveEqns method steps each wave equation object forward in time
//

void WaveEqnlist::stepWaveEqns(float **Eta, Qhistorylist* pqhistorylist, ConnectMat* pconnectmat){
  for(int i=0;i<numwaveeq;i++){
    Qhistory* pqhistory=pqhistorylist->getQhist(pconnectmat->getQindex(i));
    getwaveeq(i)->stepwaveeq(Eta[i], pqhistory);
    }
}

