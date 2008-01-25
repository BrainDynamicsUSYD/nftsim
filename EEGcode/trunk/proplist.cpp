/***************************************************************************
                          proplist.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "proplist.h"

//
// Constructor creates an array of pointers to propagator objects
//

Proplist::Proplist(int numconnects, long gridsize, float deltat):numpropag(numconnects){
  propagarray = new Propag *[numpropag];
  for(int i=0;i<numpropag;i++){
    propagarray[i] = new WaveEqn(gridsize, deltat);
  }
}

//
// Destructor deletes each propagator object and then array which holds them
//

Proplist::~Proplist(){
  for(int i=0;i<numpropag; i++)
    delete getpropag(i);
  delete [ ] propagarray;
}

//
// get method returns a pointer to the "index"th propagator object in the list
//

Propag * Proplist::getpropag(int index){
  return propagarray[index];
}

//
//  init method initializes propagator object in turn
//

void Proplist::init(Istrm& inputf){
  for(int i=0; i<numpropag; i++)
    getpropag(i)->init(inputf);
}

//
//  dump method dumps propagator objects in turn for restart
//
void Proplist::dump(ofstream& dumpf){
  for(int i=0; i<numpropag; i++){
    dumpf << "Wave Eqn "<< (i+1) << " ";
    getpropag(i)->dump(dumpf);
    }
}

void Proplist::restart(Istrm& restartf){
  for(int i=0; i<numpropag; i++)
    getpropag(i)->restart(restartf);
}

//
// step method steps each propagator object forward in time
//

void Proplist::step(float **Eta, Qhistorylist* pqhistorylist, ConnectMat* pconnectmat){
  for(int i=0;i<numpropag;i++){
    Qhistory* pqhistory=pqhistorylist->getQhist(pconnectmat->getQindex(i));
    getpropag(i)->stepwaveeq(Eta[i], pqhistory);
    }
}

