/***************************************************************************
                          dendriticrlist.cpp  -  A linked list of dendritic
                                                 response for afferent neural
                                                 populations
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include"dendriticrlist.h"
#include"connectmat.h"
#include"propagnet.h"

//
// Constructor for DendriticRList creates a linked list of dendritic responses for
// all incoming afferent neural populations
//

DendriticRlist::DendriticRlist(long nodes, int popindex, ConnectMat *pconnectmat)
	          :numnodes(nodes),popindex(popindex){
  numaff=pconnectmat->getDRlength(popindex);
  int i=numaff;
  DendriticR *currentlink;
  DendriticR *nextlink;
  firstlink = new DendriticR();
  i--;
  currentlink=firstlink;
  for(; i>0; i--){
    nextlink= new DendriticR();
    currentlink->next=nextlink;
    currentlink=nextlink;
    }
  Va = new float *[numaff];
  dVdt = new float *[numaff];
  for(int i=0;i<numaff;i++){
    Va[i] = new float[nodes];
    dVdt[i] = new float[nodes];
  }
  localP = new float*[numaff];
}

//
// Destructor sucessively deletes each object in the linked list
//

DendriticRlist::~DendriticRlist(){
  for(int i=numaff;i>0; i--)
    delete getdendr(i-1);
  for(int i=0;i<numaff;i++){
    delete[ ] Va[i];
    delete[ ] dVdt[i];
  }
  delete[ ] Va;
  delete[ ] dVdt;
  delete[ ] localP;
}

//
// get method returns a pointer to the "i"th dendritic response in the dendritic response list
//

DendriticR * DendriticRlist::getdendr(int i){
  DendriticR *currentlink;
  if(i<numaff){
    currentlink=firstlink;
    for(;i>0; i--)
      currentlink=currentlink->next;
    }
  else {
    currentlink=0;
    }
  return currentlink;
 }

void DendriticRlist::init(ifstream& inputf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
// Read data from inputf
  inputf.ignore(200,58); // Throwaway title line uptil colon
  int expectaff;
  inputf >> expectaff; // Read in number of dendritic responses expected for this population
  if (expectaff!=numaff)
    cerr << "Number of afferent populations expected from connection matrix and that stated here are inconsistent";
  for(int i=0;i<numaff; i++)
    getdendr(i)->init(inputf);
// initialize localP pointer array
  int end=ppropagnet->numconnects;
  int counter=0;
  for(int j=0; j<end; j++){
    if(pconnectmat->getDRindex(j)==popindex){ // Test if the 'j'th P array in propagnet is connected to this population
     localP[counter]=ppropagnet->P[j];
     counter++;
    }
  }
}

void DendriticRlist::dump(ofstream& dumpf){
  dumpf << "Number of Dendritic responses: " << numaff <<endl;
  for(int i=0; i<numaff; i++)
    getdendr(i)->dump(dumpf);
}

void DendriticRlist::restart(ifstream& restartf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
// Read data from restartf
  restartf.ignore(200,58); // Throwaway title line uptil colon
  int expectaff;
  restartf >> expectaff; // Read in number of dendritic responses expected for this population
  cerr << expectaff << numaff << endl;
  if (expectaff!=numaff)
    cerr << "Number of afferent populations expected from connection matrix and that stated here are inconsistent";
  for(int i=0;i<numaff; i++)
    getdendr(i)->restart(restartf);
// initialize localP pointer array
  int end=ppropagnet->numconnects;
  int counter=0;
  for(int j=0; j<end; j++){
    if(pconnectmat->getDRindex(j)==popindex){ // Test if the 'j'th P array in propagnet is connected to this population
     localP[counter]=ppropagnet->P[j];
     counter++;
    }
  }
}

//
// stepVa method integrates the dendritic response DE for each dendrite subpopulation
// Looping over the dendritic responses [0:numaff-1] update the Vab for each
// The stepVa method takes a pointer stored in localP which points to the 
// Pab data in the PropagNet class. localP was initialized in DendriticRlist::init()
// and exists to speedup the acessing of Pab data rather than looking up
// the Pab which connect to this populations dendritic tree each time stepVa is called
//

void DendriticRlist::stepVa(float timestep){
  for(int i=0;i<numaff;i++){
    getdendr(i)->stepVab(localP[i], Va[i], dVdt[i], numnodes, timestep);
    }
}

//
// SumAfferent sums the local Va matrix to obtain V array
// Physically this is summing the afferent subpotentials in the dendritites
// to obtain the soma potential
//
void DendriticRlist::SumAfferent(float *V){
  float *Voffset;
  float *Vaoffset;
  Voffset=V;
  Vaoffset=Va[0];
  for(long i=0;i<numnodes;i++)
    *Voffset++=*Vaoffset++;  // load soma potential array with first subpotential values
  for(int i=1;i<numaff;i++){
    Voffset=V;
    Vaoffset=Va[i];
    for(long j=0;j<numnodes;j++){  // add other rest of sub potential arrays to some potential arrays
      *Voffset+=*Vaoffset;
      Voffset++;
      Vaoffset++;
    }
  }
}
