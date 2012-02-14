/***************************************************************************
                          couplinglist.cpp  -  Holds a list of various
                                               synaptic coupling objects
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cstdlib>
#include<string>
using std::string;
#include<sstream>
#include"couplinglist.h"
#include"coupling.h"
#include"cadp.h"
#include"poplist.h"
using std::endl;

//
// Constructor for Couplinglist creates an array of coupling objects
//

Couplinglist::Couplinglist(Istrm& inputf, ofstream& dumpf
    ,int numconnects, long nodes, double deltat)
    : numcoup(numconnects), numnodes(nodes), deltat(deltat) {
  for( int i=0; i<numcoup; i++ ) {
    std::stringstream ss; ss<<"Couple "<<i+1<<" - ";
    string type = inputf.Find( ss.str().c_str() );
    if(type=="Map")
      couparray.push_back( new Coupling(nodes,deltat) );
    else if(type=="Calcium")
      couparray.push_back( new CaDP(nodes,deltat) );
    else {
      std::cerr << "Invalid Coupling type" << endl;
      exit(EXIT_FAILURE);
    }
  }
  string buffer = inputf.Find("Lambda:"); Lambda = atof(buffer.c_str());
  dumpf << "Lambda: " << Lambda << endl;
  buffer = inputf.Find("tGlu:"); tGlu = atof(buffer.c_str());
  dumpf << "tGlu: " << tGlu << endl;
  glu = new double[numnodes];
  for( int i=0; i<numnodes; i++ )
    glu[i] = 0;
  dglu = new double[numnodes];
  for( int i=0; i<numnodes; i++ )
    dglu[i] = 0;
}

// Destructor deletes each coupling object and then array which holds them
Couplinglist::~Couplinglist(){
  for(int i=0;i<numcoup; i++)
    delete couparray[i];
  if(glu) delete[] glu;
  if(dglu) delete[] dglu;
}

void Couplinglist::init(Istrm& inputf){
  for(int i=0; i<numcoup; i++)
    getcoup(i).init(inputf,i);
}

void Couplinglist::dump(ofstream& dumpf){
  for(int i=0; i<numcoup; i++){
    dumpf << "Coupling " << (i+1) << " ";
    getcoup(i).dump(dumpf);
    dumpf << endl; //Append endl to coupling data
  }
}

// updateP method updates P via each coupling object 
void Couplinglist::updateP(double **P,double **Eta,const Poplist& poplist,const ConnectMat& connectmat){
  for( int j=0; j<numnodes; j++ )
    dglu[j] = 0;//double ddglu = 0; double ts = 200e-3; double td = 200e-3;
  for( int i=0; i<numcoup; i++ )
    if( getcoup(i).sign )
      for( int j=0; j<numnodes; j++ )
        dglu[j] += Lambda*Eta[i][j]*deltat;
  for( int j=0; j<numnodes; j++ ) {
    dglu[j] -= glu[j]/tGlu*deltat;
    glu[j] += dglu[j];
    if( glu[j]<0 ) glu[j] = 0;
    /*double p1 = ddglu;
      double k1 = dglu[j];
      double p2 = p1 -p1/2/ts -k1/2/td;
      double k2 = dglu[j] +1/2*(dglu[j]+p1);
      double p3 = p1 -p2/2/ts -k2/2/td;
      double k3 = dglu[j] +1/2*(dglu[j]+p2);
      double p4 = p1 -p3/ts -k3/td;
      double k4 = dglu[j] +dglu[j]+p3;
      dglu[j] += 1/6*(p1+2*p2+2*p3+p4);
      glu[j] += 1/6*(k1+2*k2+2*k3+k4);
    */
  }
  for(int i=0;i<numcoup;i++)
    getcoup(i).updatePa(P[i],Eta[i],poplist.get(connectmat.getPostPop(i)).V,glu);
}

void Couplinglist::output(){
  for(int i=0;i<numcoup;i++)
    getcoup(i).output();
}
