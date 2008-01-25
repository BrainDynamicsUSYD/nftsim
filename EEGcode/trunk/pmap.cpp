/***************************************************************************
                          pmap.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "pmap.h"
#include<math.h>

Pmap::Pmap(long gsize, float dt):gridsize(gsize),deltat(dt){
  rowlength=static_cast<long>(sqrt(gridsize));
  sidelength=rowlength-2;
  startfirstrow=rowlength+1;
}


Pmap::~Pmap(){
}

void Pmap::init(Istrm& inputf){
  inputf.validate("Tauab",58);
  inputf >> tauab;
}

void Pmap::dump(ofstream& dumpf){
  dumpf << "Tau_ab: " << tauab << " ";
  dumpf << endl;
}

void Pmap::restart(Istrm& restartf){
  restartf.ignore(200,58); // throw away everything before the colon character
  restartf >> tauab;
}

void Pmap::stepwaveeq(float *Phi, Qhistory *pqhistory){
  float* Q= pqhistory->getQbytime(tauab);
  // initialize five indexes to spatial positions
  icentre=startfirstrow;
  iphi=0;
  // loop over nodes
  for(long i=0; i<sidelength; i++){
    for(long j=0; j<sidelength; j++){
      Phi[iphi]=Q[icentre];
      icentre++; // increment position indexes
      iphi++; // increment phi position index
      }
    icentre+=2; // reposition indexes to start of next row, they were already incremented
                // one space within inner loop
    }
}
