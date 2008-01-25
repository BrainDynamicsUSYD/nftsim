/***************************************************************************
                          population.cpp  -  Population object describes a
			                     neural population
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "population.h"

Population::Population(long totalnodes, int popindex, ConnectMat *pconnectmat)
             :dr(totalnodes,popindex,pconnectmat),nodes(totalnodes) {
  V = new float[totalnodes];
  Q = new float[totalnodes];
}

//
// destructor for population frees V and Q data
//
Population::~Population(){
    delete[ ] V;
    delete[ ] Q;
}

//
// init method
//
void Population::init(ifstream& inputf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
  inputf.ignore(200,32); //throwaway line naming population
  float Qsteady;
  inputf.ignore(200,58); //throwaway everything upto colon character
  inputf >> Qsteady;
  for(long i=0; i<nodes; i++)
      Q[i]=Qsteady;
  fr.init(inputf);
  dr.init(inputf,ppropagnet,pconnectmat);
  inputf.ignore(200,32); //throwaway space line between populations
// Temp fill some potential with a testing data set
  for(long i=0; i<nodes; i++)
      V[i]=(static_cast<float>(i)-50.0F)/2.0F;
}
//
// dump method gives diagnostic information on the population
//
void Population::dump(ofstream& dumpf){
  dumpf << "Population " << endl; // Write a title line for the population
  dumpf << "Q array : ";
  for(long i=0; i<nodes; i++)
    dumpf << Q[i] <<" ";
  dumpf << endl; // append a endl to end of Q array
  fr.dump(dumpf);
  dr.dump(dumpf);
}

//
// restart method
//
void Population::restart(ifstream& restartf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
  restartf.ignore(200,32); // Throwaway title line for population
  restartf.ignore(200,58); // Throwaway upto colon i.e. Q array :
  for(long i=0; i<nodes; i++)
    restartf >> Q[i];
  restartf.ignore(200,32); // Throwaway appended endl at end of Q array
  fr.restart(restartf);
  dr.restart(restartf,ppropagnet,pconnectmat);
}

//
// Step forward population one timestep method
//
void Population::stepPop(float timestep){
  dr.stepVa(timestep);
  dr.SumAfferent(V);
  fr.getQ(V,Q,nodes);
}

