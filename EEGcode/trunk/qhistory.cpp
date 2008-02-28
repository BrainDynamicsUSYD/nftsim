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
	       :indexofQ(indexQ),depthq(qdepth),nodes(totalnodes){
  qhistory = new double*[qdepth+3];
  double *q;
  for(int i=0;i<qdepth+3;i++){
    qhistory[i] = new double[nodes];
    // Initialize q arrays to zero
    q = qhistory[i];
    for(long j=0;j<nodes;j++)
      *q++=0.0F;
  }
  timeindex = new int[qdepth+3]; 
  for(int i=0;i<qdepth+3;i++) // Fill time index with default arrangement
    timeindex[i]=i;
}
Qhistory::~Qhistory(){
  for(int i=0;i<depthq+3;i++)
    delete [ ] qhistory[i];
  delete [ ] qhistory;
  delete [ ] timeindex;
}

//
// Finish by copying q throughout the q history at start
//
void Qhistory::init(Istrm& inputf, Poplist *ppoplist){
  copyQfrompop(ppoplist); //Update Q to qdepth
//
// Set Q back in time to initial conditions
//
  for(int i=0;i<depthq+2;i++){
    double *qnew = qhistory[i];
    double *q = qhistory[depthq+2];
    for(long j=0;j<nodes;j++)
      *qnew++=*q++;
  }
}

void Qhistory::dump(ofstream& dumpf){
  for(int i=0;i<depthq+3;i++){
    double *q=qhistory[i];
    dumpf << "Qhistory index " << i << " :";
    for(long j=0;j<nodes;j++)
      dumpf << *q++ <<" ";
    dumpf << endl;
  }
  dumpf << "Qhistory timeindex :";
  for(int i=0;i<depthq+3;i++){
    dumpf << timeindex[i] <<" ";
  }
  dumpf << endl;
}

void Qhistory::restart(Istrm& restartf, Poplist *ppoplist){
  for(int i=0;i<depthq+3;i++){
    double *q=qhistory[i];
    double qtemp;
    restartf.ignore(200,58); // throw away Qhistorydepth xx :
    for(long j=0;j<nodes;j++){
      restartf >> qtemp;
      *(q+j) = qtemp;
    }
  }
  restartf.ignore(200,32); // throwaway endl at end of Q history array
  restartf.ignore(200,58); // throwaway Qhistory timeindex :
  int timetemp;
  for(int i=0;i<depthq+3;i++){
    restartf >> timetemp;
    timeindex[i]=timetemp;
  }
  restartf.ignore(200,32); // throwaway endl at end of time index array
}

//
// Update Qhistory by reading in Q from population and updating keyring pointers
//
void Qhistory::updateQhistory(Poplist *ppoplist){
// First, copy the Q array incoming from Q in each population to Q array in Qhistory
  copyQfrompop(ppoplist);
// Second, increment timeindex array updating indexes of Q values
  for(int i=0;i<depthq+3;i++){
    if(timeindex[i]!=0)
      timeindex[i]--;
    else
      timeindex[i]=depthq+2;
  }
}

void Qhistory::copyQfrompop(Poplist *ppoplist){
// Copy the Q array incoming from Q in each population to Q array in Qhistory
  double * pnewq=getQbytime(depthq+2); // Get pointer to start of the oldest Q array which is going to be overwritten
  double * pQpop=ppoplist->get(indexofQ)->Q; // Get pointer to incoming Q data originating from Q in each population
  for(long i=0; i<nodes; i++){
      *pnewq++=*pQpop++;
  }
}
