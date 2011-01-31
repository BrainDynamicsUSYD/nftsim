/***************************************************************************
                          qhistory.cpp  -  keyring storing q into past
                             to account for propagation delays along axons
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
#include<math.h>
#include<cstdlib>
#include"qhistory.h"
#include"poplist.h"
using std::endl;

Qhistory::Qhistory(long n, int indexQ):indexofQ(indexQ),nodes(n){
  for(int i=0;i<3;i++){ // we need a minimum of three time steps back
    qhistory.push_back(new double[nodes]);
    // Initialize q arrays to zero
    double *q = qhistory[i];
    for(long j=0;j<nodes;j++){
      *q++=0.0F;
    }
  }
  inew=0;
}
Qhistory::~Qhistory(){
  while( !qhistory.empty() ){
    delete [ ] qhistory.back();
    qhistory.pop_back();
  }
}

void Qhistory::grow(int taumax){
  uint olddepth=qhistory.size();
  while( taumax>( static_cast<int>(qhistory.size())-3) ){
    qhistory.push_back(new double[nodes]);
  }
// initialize the new part of the qhistory array
  for(uint i=olddepth;i<(qhistory.size()-1);i++){
    double * __restrict__ qnew = qhistory[i];
    double * __restrict__ q = qhistory[olddepth-1];
    for(long j=0;j<nodes;j++)
      *qnew++=*q++;
  }
}

void Qhistory::init(Istrm& inputf,Poplist& poplist){
  copyQfrompop(poplist); //Update Q to qdepth
// Set Q back in time to initial conditions
  for(std::vector<double *>::iterator it=qhistory.begin();it!=qhistory.end();++it){
    double * __restrict__ qnew = *it;
    double * __restrict__ q = qhistory[qhistory.size()-1];
    for(long j=0;j<nodes;j++)
      *qnew++=*q++;
  }
}

void Qhistory::dump(ofstream& dumpf){
  dumpf << "Qhistory depth: " << qhistory.size() << " ";
  for(uint i=0;i<qhistory.size();i++){
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
  restartf.validate("Qhistory depth",58);
  int depth;
  restartf >> depth;
  grow(depth-3);  // ensure qhistory has correct depth (-3) is different convention between taumax and size
  for(std::vector<double *>::iterator it=qhistory.begin();it!=qhistory.end();++it){
    double *q=*it;
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
  ++inew; // Increment position of newest Qhistory
  if( static_cast<int>(qhistory.size()) == inew)inew=0;
}

void Qhistory::copyQfrompop(Poplist& poplist){
// Copy the Q array incoming from Q in each population to Q array in Qhistory
  double * __restrict__ pnewq=getQbytime(qhistory.size()-1); // Get pointer to start of the oldest Q array which is going to be overwritten
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
      *qarr=qhistory[*tarray<inew?qhistory.size()+*tarray-inew:*tarray-inew][j];
    }
    return tauobj.qarray;
  } else {
    return qhistory[tauobj.tauab<inew?qhistory.size()+tauobj.tauab-inew:tauobj.tauab-inew];
  }
}; // Get a pointer to the Q array with parameters of object tau
