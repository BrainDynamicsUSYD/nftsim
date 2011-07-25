/***************************************************************************
                          couplinglist.cpp  -  Holds a list of various
                                               synaptic coupling objects
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cstdlib>
#include"couplinglist.h"
#include"coupling.h"
#include"modcouple.h"
#include"stdp.h"
#include"cadp.h"
using std::endl;

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
    optionnum=inputf.choose("Simple:1 Modulate:2 STDP:3 Calcium:4",32);
    if(1==optionnum){
      couparray[i] = new Coupling(nodes,deltat);
      dumpf << (i+1) << ": Simple ";
    }
    if(2==optionnum){
      couparray[i] = new Modcouple(nodes,deltat);
      dumpf << (i+1) << ": Modulate ";
    }
    if(3==optionnum){
      couparray[i] = new STDP(nodes,deltat);
      dumpf << (i+1) << ": STDP ";
    }
    if(4==optionnum){
      couparray[i] = new CaDP(nodes,deltat);
      dumpf << (i+1) << ": Calcium ";
    }
    if( optionnum<1 || optionnum>4 ){
      std::cerr << "Invalid Coupling type" << endl;
      exit(EXIT_FAILURE);
    }
  }
  numnodes = nodes;
  this->deltat = deltat;
  glu = new double[numnodes];
  for( int i=0; i<numnodes; i++ )
    glu[i] = 0;
}

// Destructor deletes each coupling object and then array which holds them
Couplinglist::~Couplinglist(){
  for(int i=0;i<numcoup; i++)
    delete getcoup(i);
  delete [ ] couparray;
  if(glu) delete[] glu;
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
void Couplinglist::updateP(double **P,double **Eta,Qhistorylist& qhistorylist,ConnectMat& connectmat){
  //for( int j=0; j<numnodes; j++ )
      //glu[j] = 0;
  double dglu;
  for(int i=0;i<numcoup;i++){
    if( getcoup(i)->sign )
      for( int j=0; j<numnodes; j++ ) {
        dglu = ( 5e-7*Eta[i][j] -glu[j]/200e-3 )*deltat;
    	if( glu[j]+dglu < 0 )
          glu[j] = 0;
    	else
          glu[j] += dglu;
      }
  }
  for(int i=0;i<numcoup;i++){
    getcoup(i)->updatePa(P[i],Eta[i],qhistorylist,connectmat,*this);
  }
}

void Couplinglist::output(){
  for(int i=0;i<numcoup;i++){
    getcoup(i)->output();
  }
}
