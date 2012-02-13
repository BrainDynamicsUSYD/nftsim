/***************************************************************************
                          dendriticrlist.cpp  -  Aan array of dendritic
                                                 response for afferent neural
                                                 populations
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cstdlib>
#include"dendriticrlist.h"
#include"connectmat.h"
#include"propagnet.h"
using std::endl;

//
// Constructor creates an array of dendritic responses for
// all incoming afferent neural populations
//

DendriticRlist::DendriticRlist(long nodes, int popid,ConnectMat& connectmat)
	          :numnodes(nodes),popindex(popid){
  numaff=connectmat.getDRlength(popindex);
  drarray = new DendriticR *[numaff];
  for(int i=0;i<numaff;i++){
    drarray[i] = new DendriticR(nodes);
  }
  Va = new double *[numaff];
  dVdt = new double *[numaff];
  for(int i=0;i<numaff;i++){
    Va[i] = new double[nodes];
    dVdt[i] = new double[nodes];
  }
  localP = new double*[numaff];
}

// Destructor deletes each DendriticR object and then array holding them
DendriticRlist::~DendriticRlist(){
  for(int i=0;i<numaff;i++)
    delete &getdendr(i);
  delete [ ] drarray;
  for(int i=0;i<numaff;i++){
    delete[ ] Va[i];
    delete[ ] dVdt[i];
  }
  delete[ ] Va;
  delete[ ] dVdt;
  delete[ ] localP;
}

// reference to the "index"th dendritic response in the dendritic response list
DendriticR& DendriticRlist::getdendr(int index){
  return *drarray[index];
}

void DendriticRlist::init(Istrm& inputf,PropagNet& propagnet,ConnectMat& connectmat){
// initialize localP pointer array
  int end=propagnet.numconnects;
  int counter=0;
  for(int j=0; j<end; j++){
    if(connectmat.getPostPop(j)==popindex){ // Test if the 'j'th P array in propagnet is connected to this population
     localP[counter]=propagnet.P[j];
     counter++;
    }
  }
// Read data from inputf
  int j=0;
  for(int i=0;i<numaff; i++,j++){
    while(connectmat.getPostPop(j)!=popindex) j++;
      double Vinit;
      getdendr(i).init(inputf,Vinit,j,connectmat.getPrePop(j)); // Important init returns inital values for V, dVdt
      double * __restrict__ p=Va[i];
      double * __restrict__ p1=dVdt[i];
      long n=numnodes;
      double Vi=Vinit;
    for(long j=0;j<n;j++) *p++=Vi; // Initialize Va arrays with initial values
    for(long j=0;j<n;j++) *p1++=0.0F;// Init dVdt arrays with initial values
  }
}

void DendriticRlist::dump(ofstream& dumpf){
  dumpf << "Number of Dendritic responses: " << numaff <<endl;
  for(int i=0; i<numaff; i++)
    getdendr(i).dump(dumpf);
  dumpf << "Va, dVdt data:";
  for(int i=0;i<numaff; i++){
    double *p=Va[i];
    double *p1=dVdt[i];
    for(long j=0; j<numnodes; j++){
      dumpf << p[j] << " " << p1[j] << " ";
    }
  }
}

void DendriticRlist::restart(Istrm& restartf,PropagNet& propagnet,ConnectMat& connectmat){
// Read data from restartf
  restartf.ignore(58); // Throwaway title line uptil colon
  for(int i=0;i<numaff; i++)
    getdendr(i).restart(restartf);
// initialize localP pointer array
  int end=propagnet.numconnects;
  int counter=0;
  for(int j=0; j<end; j++){
    if(connectmat.getPostPop(j)==popindex){ // Test if the 'j'th P array in propagnet is connected to this population
     localP[counter]=propagnet.P[j];
     counter++;
    }
  }
  /*restartf.validate("Va, dVdt data",58);
  double tempV;
  double tempdVdt;
  for(int i=0;i<numaff; i++){
    double *p=Va[i];
    double *p1=dVdt[i];
    for(long j=0; j<numnodes; j++){
      restartf >> tempV;
      restartf >> tempdVdt;
      p[j]=tempV;
      p1[j]=tempdVdt;
    }
  }*/
  
}

//
// stepVa method integrates the dendritic response DE for each dendrite subpopulation
// Looping over the dendritic responses [0:numaff-1] update the Vab for each
// The stepVa method takes a pointer stored in localP which points to the 
// Pab data in the PropagNet class. localP was initialized in DendriticRlist::init()
// and exists to speedup the acessing of Pab data rather than looking up
// the Pab which connect to this populations dendritic tree each time stepVa is called
//

void DendriticRlist::stepVa(double timestep){
//
//#pragma omp parallel for
//
  for(int i=0;i<numaff;i++){
    getdendr(i).stepVab(localP[i], Va[i], dVdt[i],timestep);
  }
}

//
// SumAfferent sums the local Va matrix to obtain V array
// Physically this is summing the afferent subpotentials in the dendritites
// to obtain the soma potential
//
void DendriticRlist::SumAfferent(double * __restrict__ V){
  double * __restrict__ Vout=V;
  double * __restrict__ Vin=Va[0];
  long n=numnodes;
  for(long i=0;i<n;i++)
    Vout[i]=Vin[i];  // load soma potential array with first subpotential values
  for(int i=1;i<numaff;i++){
    double * __restrict__ Vo=V;
    double * __restrict__ Vi=Va[i];
    for(long j=0;j<n;j++){  // add other rest of sub potential arrays to some potential arrays
      Vo[j] += Vi[j];
    }
  }
}