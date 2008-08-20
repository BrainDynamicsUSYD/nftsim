/***************************************************************************
                          qhistory.cpp  -  keyring storing q into past
                             to account for propagation delays along axons
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
#include<math.h>

#include"qhistory.h"
#include"poplist.h"

Qhistory::Qhistory(int qdepth, long totalnodes, int indexQ)
	       :indexofQ(indexQ),depth(qdepth+3),nodes(totalnodes){
  qhistory = new double*[depth];
  double *q;
  for(int i=0;i<depth;i++){
    qhistory[i] = new double[nodes];
    // Initialize q arrays to zero
    q = qhistory[i];
    for(long j=0;j<nodes;j++)
      *q++=0.0F;
  }
  inew=0;
}
Qhistory::~Qhistory(){
  for(int i=0;i<depth;i++)
    delete [ ] qhistory[i];
  delete [ ] qhistory;
}

//
// Finish by copying q throughout the q history at start
//
void Qhistory::init(Istrm& inputf, Poplist *ppoplist){
  copyQfrompop(ppoplist); //Update Q to qdepth
//
// Set Q back in time to initial conditions
//
  for(int i=0;i<depth-1;i++){
    double *qnew = qhistory[i];
    double *q = qhistory[depth-1];
    for(long j=0;j<nodes;j++)
      *qnew++=*q++;
  }
}

void Qhistory::dump(ofstream& dumpf){
  for(int i=0;i<depth;i++){
    double *q=qhistory[i];
    dumpf << "Qhistory index " << i << " :";
    for(long j=0;j<nodes;j++)
      dumpf << *q++ <<" ";
    dumpf << endl;
  }
  dumpf << "Qhistory inew:" << inew << " ";
  dumpf << endl;
}

void Qhistory::restart(Istrm& restartf, Poplist *ppoplist){
  for(int i=0;i<depth;i++){
    double *q=qhistory[i];
    double qtemp;
    restartf.ignore(200,58); // throw away Qhistorydepth xx :
    for(long j=0;j<nodes;j++){
      restartf >> qtemp;
      *(q+j) = qtemp;
    }
  }
  restartf.ignore(200,32); // throwaway endl at end of Q history array
  restartf.ignore(200,58); // throwaway Qhistory inew :
  restartf >> inew;
  restartf.ignore(200,32); // throwaway endl at end of time index array
}

//
// Update Qhistory by reading in Q from population and updating keyring pointers
//
void Qhistory::updateQhistory(Poplist *ppoplist){
// First, copy the Q array incoming from Q in each population to Q array in Qhistory
  copyQfrompop(ppoplist);
  inew++; // Increment position of newest Qhistory
  if(depth==inew)inew=0;
}

void Qhistory::copyQfrompop(Poplist *ppoplist){
// Copy the Q array incoming from Q in each population to Q array in Qhistory
  double * pnewq=getQbytime(depth-1); // Get pointer to start of the oldest Q array which is going to be overwritten
  double * pQpop=ppoplist->get(indexofQ)->Q; // Get pointer to incoming Q data originating from Q in each population
  for(long i=0; i<nodes; i++){
      *pnewq++=*pQpop++;
  }
}
