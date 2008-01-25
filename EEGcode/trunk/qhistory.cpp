/***************************************************************************
                          qhistory.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<iostream>
#include<math.h>

#include"qhistory.h"
#include"poplist.h"

Qhistory::Qhistory(int qdepth, long gsize, int indexQ)
	       :indexofQ(indexQ),depthq(qdepth),gridsize(gsize){
  qhistory = new float*[qdepth+3];
  float *q;
  for(int i=0;i<qdepth+3;i++){
    qhistory[i] = new float[gridsize];
    // Initialize q arrays to zero
    q = qhistory[i];
    for(long j=0;j<gridsize;j++)
      *q++=0.0F;
  }
  timeindex = new int[qdepth+3]; 
  for(int i=0;i<qdepth+3;i++) // Fill time index with default arrangement
    timeindex[i]=i;
  rowlength=static_cast<long>(sqrt(gridsize));
  sidelength=rowlength-2;
  startfirstrow=rowlength+1;
  startlastrow=rowlength*sidelength+1;
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
void Qhistory::init(ifstream& inputf, Poplist *ppoplist){
  copyQfrompop(ppoplist); //Update Q to qdepth
//
// Set Q back in time to initial conditions
//
  for(int i=0;i<depthq+2;i++){
    float *qnew = qhistory[i];
    float *q = qhistory[depthq+2];
    for(long j=0;j<gridsize;j++)
      *qnew++=*q++;
  }
}

void Qhistory::dump(ofstream& dumpf){
  for(int i=0;i<depthq+3;i++){
    float *q=qhistory[i];
    dumpf << "Qhistory index " << i << " :";
    for(long j=0;j<gridsize;j++)
      dumpf << *q++ <<" ";
    dumpf << endl;
  }
  dumpf << "Qhistory timeindex :";
  for(int i=0;i<depthq+3;i++){
    dumpf << timeindex[i] <<" ";
  }
  dumpf << endl;
}

void Qhistory::restart(ifstream& restartf, Poplist *ppoplist){
  for(int i=0;i<depthq+3;i++){
    float *q=qhistory[i];
    float qtemp;
    restartf.ignore(200,58); // throw away Qhistorydepth xx :
    for(long j=0;j<gridsize;j++){
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
// First, copy the Q array incoming from Q in each population to Q array in Qhistory which is larger sized array due to periodic boundary conditions
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
// First, copy the Q array incoming from Q in each population to Q array in Qhistory which is larger sized array due
// to periodic boundary conditions
  float * pnewq=getQbytime(depthq+2); // Get pointer to start of the oldest Q array which is going to be overwritten
  float * pQpop=ppoplist->get(indexofQ)->Q; // Get pointer to incoming Q data originating from Q in each population
// Next part copies middle of Q values grid across
  float * pq1=pnewq+startfirstrow;
  float * pq=pQpop;
  for(long i=0; i<sidelength;i++){
    for(long j=0; j<sidelength;j++){
      *pq1++=*pq++;
    }
    pq1+=2; // reposition at start of next row
  }
// Next part copies top line based on last row
  pq1=pnewq+1;
  pq=pQpop+ sidelength*(sidelength-1);
  for(long i=0; i<sidelength;i++)
     *pq1++=*pq++;
// Next part copies bottom line based on first row
  pq1=pnewq+startlastrow+rowlength;
  pq=pQpop;
  for(long i=0; i<sidelength;i++)
     *pq1++=*pq++;
// Next part copies left line based on last column 
  pq1=pnewq+startfirstrow-1;
  pq=pQpop+sidelength-1;
  for(long i=0; i<sidelength;i++){
     *pq1=*pq;
     pq1+=rowlength;
     pq+=sidelength;
  }
// Next part copies right line based on first column 
  pq1=pnewq+startfirstrow+sidelength;
  pq=pQpop;
  for(long i=0; i<sidelength;i++){
     *pq1=*pq;
     pq1+=rowlength;
     pq+=sidelength;
  }
}
