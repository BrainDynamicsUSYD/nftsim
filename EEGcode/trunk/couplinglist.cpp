/***************************************************************************
                          couplinglist.cpp  -  Holds a list of various
                                               synaptic coupling objects
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include"couplinglist.h"
#include"coupling.h"
#include"modcouple.h"
#include"coupleplast.h"

//
// Constructor for Couplinglist creates an array of coupling objects
//

Couplinglist::Couplinglist(Istrm& inputf, ofstream& dumpf
  ,int numconnects, long nodes, double deltat):numcoup(numconnects){
  couparray = new Couple *[numcoup];
  inputf.validate("Coupling type",115);// search for "propagator types NB:- the s is ASCII 115
  dumpf << "Coupling types ";
  int optionnum;
  for(int i=0;i<numcoup;i++){
    inputf.ignore(200,58);
    optionnum=inputf.choose("Simple:1 Modulate:2 Plastic:3 ",32);
    if(1==optionnum){
      couparray[i] = new Coupling(nodes,deltat);
      dumpf << (i+1) << ": Simple ";
    }
    if(2==optionnum){
      couparray[i] = new Modcouple(nodes,deltat);
      dumpf << (i+1) << ": Modulate ";
    }
    if(3==optionnum){
      couparray[i] = new Coupleplast(nodes,deltat);
      dumpf << (i+1) << ": Plastic ";
    }
    if(1!=optionnum && 2!=optionnum && 3!=optionnum){
      cerr << "Invalid Coupling type" << endl;
      exit(EXIT_FAILURE);
    }
  }
}

// Destructor deletes each coupling object and then array which holds them
Couplinglist::~Couplinglist(){
  for(int i=0;i<numcoup; i++)
    delete getcoup(i);
  delete [ ] couparray;
}

void Couplinglist::init(Istrm& inputf){
  for(int i=0; i<numcoup; i++)
    getcoup(i)->init(inputf,i);
}

void Couplinglist::dump(ofstream& dumpf){
  for(int i=0; i<numcoup; i++){
    dumpf << "Coupling " << (i+1) << " ";
    getcoup(i)->dump(dumpf);
    dumpf << endl; //Append endl to coupling data
  }
}

// updateP method updates P via each coupling object 
void Couplinglist::updateP(double **P,double **Eta,Qhistorylist* pqhistorylist,ConnectMat* pconnectmat){
  for(int i=0;i<numcoup;i++){
    getcoup(i)->updatePa(P[i],Eta[i],pqhistorylist,pconnectmat);
    }
}

void Couplinglist::output(){
  for(int i=0;i<numcoup;i++){
    getcoup(i)->output();
    }
}
