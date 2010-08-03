/***************************************************************************
                          qhistory.cpp  -  keyring storing q into past
                             to account for propagation delays along axons
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
#include<math.h>
#include<cstdlib>

#include"qhistory.h"
#include"poplist.h"

Qhistory::Qhistory(int qdepth,long n, int indexQ)
	       :indexofQ(indexQ),depth(qdepth+3),nodes(n){
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
void Qhistory::init(Istrm& inputf,Poplist& poplist){
  copyQfrompop(poplist); //Update Q to qdepth
//
// Set Q back in time to initial conditions
//
  for(int i=0;i<depth-1;i++){
    double * __restrict__ qnew = qhistory[i];
    double * __restrict__ q = qhistory[depth-1];
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

void Qhistory::restart(Istrm& restartf,Poplist& poplist){
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
void Qhistory::updateQhistory(Poplist& poplist){
// First, copy the Q array incoming from Q in each population to Q array in Qhistory
  copyQfrompop(poplist);
  inew++; // Increment position of newest Qhistory
  if(depth==inew)inew=0;
}

void Qhistory::copyQfrompop(Poplist& poplist){
// Copy the Q array incoming from Q in each population to Q array in Qhistory
  double * __restrict__ pnewq=getQbytime(depth-1); // Get pointer to start of the oldest Q array which is going to be overwritten
  double * __restrict__ pQpop=poplist.get(indexofQ).Q; // Get pointer to incoming Q data originating from Q in each population
  for(long i=0; i<nodes; i++){
      *pnewq++=*pQpop++;
  }
}
  
double * Qhistory::getQbytime(Tau& tauobj){
  if(tauobj.isarraytau){
    long* tarray=tauobj.tauarr;
    double* qarr=tauobj.qarray;
    for(long j=0;j<nodes;j++,tarray++,qarr++){
      *qarr=qhistory[*tarray<inew?depth+*tarray-inew:*tarray-inew][j];
    }
    return tauobj.qarray;
  } else {
    return qhistory[tauobj.tauab<inew?depth+tauobj.tauab-inew:tauobj.tauab-inew];
  }
}; // Get a pointer to the Q array with parameters of object tau
