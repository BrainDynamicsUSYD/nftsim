/***************************************************************************
                          coupling.cpp  -  description
                             -------------------
   copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "coupling.h"
using std::endl;

Coupling::Coupling(long numnodes, double deltat)
  :nodes(numnodes){
}

Coupling::~Coupling(){
}

void Coupling::init(Configf& inputf, int coupleid){
  inputf.Param("Nu",nu);
  if( nu>0 )
    sign = 1;
  else
    sign = -1;
}

void Coupling::dump(ofstream& dumpf){
  dumpf << "Nu: " << nu << endl;
}

void Coupling::updatePa(double * __restrict__ Pa,double * __restrict__ Etaa,double const *postV,double const *glu){
  for(int i=0; i<nodes; i++)
    Pa[i]=nu*Etaa[i];
}

void Coupling::output(){
}
