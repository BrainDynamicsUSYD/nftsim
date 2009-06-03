/***************************************************************************
                          proplist.cpp  -  list of propagators connecting neural
                                           populations
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "proplist.h"

//
// Constructor creates an array of pointers to propagator objects
//

Proplist::Proplist(Istrm& inputf, ofstream& dumpf, int numconnects,
            long nodes, double deltat):numpropag(numconnects){
  propagarray = new Propag *[numpropag];
  inputf.validate("Propagator type",115); // search for "propagator types NB:- the s is ASCII 115
  dumpf << "Propagator types ";
  int optionnum;
  for(int i=0;i<numpropag;i++){
    inputf.ignore(200,58);
    long longside; // declare longside now in case we use it below
    optionnum=inputf.choose(
        "Waveeqn:1 Mapping:2 Eqnset:3 Waveeqnrect:4 Eqnsetrect:5 Harmonic:6",32);
    switch(optionnum){
      case 1:
        propagarray[i] = new WaveEqn(nodes, deltat);
        dumpf << (i+1) << ": Waveeqn ";
	break;
      case 2:
        propagarray[i] = new Pmap(nodes, deltat);
        dumpf << (i+1) << ": Mapping ";
        break;
      case 3:
        propagarray[i] = new Eqnset(nodes, deltat);
        dumpf << (i+1) << ": Eqnset ";
	break;
      case 4:
        inputf.validate("Longside",58);
	inputf >> longside;
        propagarray[i] = new WaveEqn(nodes,deltat,longside);
	dumpf << (i+1) << ": Waveeqnrect ";
	dumpf << "(Longside:" << longside <<") ";
	break;
      case 5:
        inputf.validate("Longside",58);
	inputf >> longside;
        propagarray[i] = new Eqnset(nodes,deltat,longside);
        dumpf << (i+1) << ": Eqnsetrect ";
	dumpf << "(Longside:" << longside <<") ";
	break;	
      case 6:
        propagarray[i] = new Pharmonic(nodes, deltat);
        dumpf << (i+1) << ": Harmonic ";
	break;
      default:
        std::cerr << "Invalid Propagator type" << endl;
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

// get method returns a pointer to the "index"th propagator object in the list
Propag * Proplist::getpropag(int index){
  return propagarray[index];
}

void Proplist::init(Istrm& inputf, Qhistorylist* pqhistorylist, ConnectMat* pconnectmat){
  for(int i=0; i<numpropag; i++){
    Qhistory* pqhistory=pqhistorylist->getQhist(pconnectmat->getQindex(i));
    getpropag(i)->init(inputf,pqhistory);
  }
}

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

// step method steps each propagator object forward in time
void Proplist::step(double **Eta, Qhistorylist* pqhistorylist, ConnectMat* pconnectmat){
//
#pragma omp parallel for
//
  for(int i=0;i<numpropag;i++){
    Qhistory* pqhistory=pqhistorylist->getQhist(pconnectmat->getQindex(i));
    getpropag(i)->stepwaveeq(Eta[i],pqhistory);
    }
}
