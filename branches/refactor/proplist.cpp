/***************************************************************************
                          proplist.cpp  -  list of propagators

                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

//#include<cstdlib>
#include<cmath>
#include<string>
using std::string;
#include<sstream>
#include "proplist.h"
using std::endl;

// Constructor creates an array of pointers to propagator objects
Proplist::Proplist(Configf& inputf, std::ofstream& dumpf, int numconnects,
            long nodes, double deltat)
  :numpropag(numconnects)
{
  for(int i=0;i<numpropag;i++) {
    std::stringstream ss; ss<<"Propag "<<i+1<<" - ";
    string type = inputf.Find( ss.str().c_str() );
    if(type=="Wave")
      propagarray.push_back( new WaveEqn(nodes, deltat,
        (static_cast<long>(sqrt(nodes))) ) );
    else if(type=="Map")
      propagarray.push_back( new Pmap(nodes, deltat) );
    else if(type=="Eqnset")
      propagarray.push_back( new Eqnset(nodes, deltat,
        (static_cast<long>(sqrt(nodes))) ) );
    else if(type=="Harmonic")
      propagarray.push_back( new Pharmonic(nodes, deltat) );
    else if(type=="WaveRect") {
      long longside; inputf.Param("Longside",longside);
      propagarray.push_back( new WaveEqn(nodes,deltat,longside) );
    }
    else if(type=="EqnsetRect") {
      long longside; inputf.Param("Longside",longside);
      propagarray.push_back( new Eqnset(nodes,deltat,longside) );
    }
    else {
      std::cerr << "Invalid Propagator type" << endl;
      exit(EXIT_FAILURE);
    }
  }
}

// Destructor deletes each propagator object and then array which holds them
Proplist::~Proplist(){
  for(int i=0;i<numpropag; i++)
    delete &getpropag(i);
}

// get method returns a pointer to the "index"th propagator object in the list
inline Propag& Proplist::getpropag(int index){ return *propagarray[index]; }

void Proplist::init(Configf& inputf,Qhistorylist& qhistorylist,ConnectMat& connectmat){
  for(int i=0; i<numpropag; i++){
    Qhistory& qhistory=qhistorylist.getQhist(connectmat.getPrePop(i));
    getpropag(i).init(inputf,qhistory);
  }
}

void Proplist::dump(std::ofstream& dumpf){
  for(int i=0; i<numpropag; i++){
    dumpf << "Propagator "<< (i+1) << " ";
    getpropag(i).dump(dumpf);
    }
}

void Proplist::restart(Configf& restartf,Qhistorylist& qhistorylist,ConnectMat& connectmat){
  for(int i=0; i<numpropag; i++){
    Qhistory& qhistory=qhistorylist.getQhist(connectmat.getPrePop(i));
    getpropag(i).restart(restartf,qhistory);
  }
}

// steps each propagator object forward in time
void Proplist::step(double **Eta,Qhistorylist& qhistorylist,ConnectMat& connectmat){
#pragma omp parallel for
  for(int i=0;i<numpropag;i++){
    getpropag(i).stepwaveeq(Eta[i],qhistorylist.getQhist(connectmat.getPrePop(i)));
    }
}
