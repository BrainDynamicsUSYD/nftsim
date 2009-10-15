/***************************************************************************
                          coupleplast.cpp  -  Implements a neuronal plasticity
                                              model of Robinson (2009)
                             -------------------
   copyright            : (C) 2009 
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include <math.h>
#include<string>
using std::string;
#include<sstream>
using std::stringstream;
#include "coupleplast.h"

Coupleplast::Coupleplast(long numnodes, double deltat)
  :t(0),nodes(numnodes),timestep(deltat){
}

Coupleplast::~Coupleplast(){
}

void Coupleplast::init(Istrm& inputf, int coupleid){
// Initialise any variables
}

void Coupleplast::dump(ofstream& dumpf){
// Output any variables required to do a restart
}

void Coupleplast::output(){
// Output variables for data analysis
}

//
// Sum the coupling terms transforming Phi_{ab} to P_{ab}
//
void Coupleplast::updatePa(double *Pa, double *Etaa,Qhistorylist& qhistorylist,ConnectMat& connectmat){
}
