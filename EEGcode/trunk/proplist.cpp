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

Proplist::Proplist(Istrm& inputf, ofstream& dumpf, int numconnects,
            long gridsize, double deltat):numpropag(numconnects){
  propagarray = new Propag *[numpropag];
  inputf.validate("Propagator type",115); // search for "propagator types NB:- the s is ASCII 115
  dumpf << "Propagator types ";
  int optionnum;
  for(int i=0;i<numpropag;i++){
    inputf.ignore(200,58);
    optionnum=inputf.choose("Waveeqn:1 Mapping:2 Eqnset:3 ",32);
    if(1==optionnum){
      propagarray[i] = new WaveEqn(gridsize, deltat);
      dumpf << (i+1) << ": Waveeqn ";
    }
    if(2==optionnum){
      propagarray[i] = new Pmap(gridsize, deltat);
      dumpf << (i+1) << ": Mapping ";
    }
    if(3==optionnum){
      propagarray[i] = new Eqnset(gridsize, deltat);
      dumpf << (i+1) << ": Eqnset ";
    }
    if(1!=optionnum && 2!=optionnum && 3!=optionnum){
      cerr << "Invalid Propagator type" << endl;
      exit(EXIT_FAILURE);
    }
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
    dumpf << "Propagator "<< (i+1) << " ";
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

void Proplist::step(double **Eta, Qhistorylist* pqhistorylist, ConnectMat* pconnectmat){
  for(int i=0;i<numpropag;i++){
    Qhistory* pqhistory=pqhistorylist->getQhist(pconnectmat->getQindex(i));
    getpropag(i)->stepwaveeq(Eta[i], pqhistory);
    }
}

