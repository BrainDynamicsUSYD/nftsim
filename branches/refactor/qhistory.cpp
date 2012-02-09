/***************************************************************************
                          qhistory.cpp  -  keyring storing q into past
                             to account for propagation delays along axons
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
#include<vector>
using std::vector;
#include"qhistory.h"
#include"poplist.h"
using std::endl;

Qhistory::Qhistory(long n, int indexQ)
    : indexofQ(indexQ), nodes(n)
{
  for( int i=0; i<3; i++) { // we need a minimum of three time steps back
    qhistory.push_back(new double[nodes]);
    for(long j=0;j<nodes;j++)
      qhistory[i][j] = 0.0F;
  }
  inew=0;
}

Qhistory::~Qhistory()
{
  while( !qhistory.empty() ) {
    delete [] qhistory.back();
    qhistory.pop_back();
  }
}

void Qhistory::grow(int taumax)
{
  int olddepth = qhistory.size();
  while( taumax>( static_cast<int>(qhistory.size())-3) )
    qhistory.push_back(new double[nodes]);

  // initialize the new part of the qhistory array
  for(uint i=olddepth; i<(qhistory.size()-1); i++) {
    double* __restrict__ qnew = qhistory[i];
    double* __restrict__ q = qhistory[olddepth-1];
    for(long j=0;j<nodes;j++)
      *qnew++=*q++;
  }
}

void Qhistory::init(Istrm& inputf,Poplist& poplist)
{
  // copy the Q array incoming from Q in each population to Q array in Qhistory
  // Get pointer to start of the oldest Q array which is going to be overwritten
  double* __restrict__ pnewq=getQbytime(qhistory.size()-1);
  // Get pointer to incoming Q data originating from Q in each population
  double* __restrict__ pQpop=poplist.get(indexofQ).Q;
  for( long i=0; i<nodes; i++ )
    pnewq[i]=pQpop[i];
  // Set Q back in time to initial conditions
  for(std::vector<double *>::iterator it=qhistory.begin();
      it!=qhistory.end();++it){
    double * __restrict__ qnew = *it;
    double * __restrict__ q = qhistory[qhistory.size()-1];
    for(long j=0;j<nodes;j++)
      *qnew++=*q++;
  }
}

void Qhistory::dump(ofstream& dumpf)
{
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

void Qhistory::restart(Istrm& restartf,Poplist& poplist)
{
  /*int depth; restartf.Param("Qhistory depth",depth);
  grow(depth-3);  // ensure qhistory has correct depth (-3) is different convention between taumax and size
  for(std::vector<double *>::iterator it=qhistory.begin();it!=qhistory.end();++it){
    double *q=*it;
    double qtemp;
    restartf.ignore(58); // throw away Qhistorydepth xx :
    for(long j=0;j<nodes;j++){
      //restartf >> qtemp;
      *(q+j) = qtemp;
    }
  }
  restartf.ignore(32); // throwaway endl at end of Q history array
  restartf.ignore(58); // throwaway Qhistory inew :
  //restartf >> inew;
  restartf.ignore(32); // throwaway endl at end of time index array*/
}

// Update Qhistory by reading in Q from population and updating keyring pointers
void Qhistory::updateQhistory(Poplist& poplist)
{
  // copy the Q array incoming from Q in each population to Q array in Qhistory
  // Get pointer to start of the oldest Q array which is going to be overwritten
  double* __restrict__ pnewq=getQbytime(qhistory.size()-1);
  // Get pointer to incoming Q data originating from Q in each population
  double* __restrict__ pQpop=poplist.get(indexofQ).Q;
  for( long i=0; i<nodes; i++ )
    pnewq[i]=pQpop[i];
  ++inew; // Increment position of newest Qhistory
  if( static_cast<int>(qhistory.size()) == inew)
    inew = 0;
}

// Get a pointer to the Q array with parameters of object tau
double* Qhistory::getQbytime(Tau& tauobj)
{
  if( tauobj.tauarr.empty() )
    return qhistory[tauobj.tauab<inew?qhistory.size()+tauobj.tauab-inew:tauobj.tauab-inew];
  else {
    for( long i=0; i<nodes; i++ )
      tauobj.qarray[i] = qhistory[ tauobj.tauarr[i]<inew? qhistory.size()+tauobj.tauarr[i]-inew : tauobj.tauarr[i]-inew ][i];
    /*long* tarray=tauobj.tauarr;
    double* qarr=tauobj.qarray;
    for( long i=0; i<nodes; i++,tarray++,qarr++ )
      *qarr=qhistory[*tarray<inew?qhistory.size()+*tarray-inew:*tarray-inew][i];*/
    return tauobj.qarray;
  }
};
