/***************************************************************************
                          population.cpp  -  Population object describes a
			                     neural population
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "population.h"
#include "connectmat.h"

Population::Population(long totalnodes, int popindex, ConnectMat *pconnectmat)
             :t(0),nodes(totalnodes) {
  Q = new double[totalnodes];
  isstimulus=true;
  V = 0;
  pfr = 0;
  pdr = 0;
  pstimulus = 0;
  if (pconnectmat->getDRlength(popindex)) { //If populations are attached to this dendritic tree it is not a stimulus population
    isstimulus=false;
    V = new double[totalnodes];
    pfr = new FiringR(popindex);
    pdr = new DendriticRlist(totalnodes,popindex,pconnectmat);
  } else {
  }
}

Population::~Population(){
  delete[ ] Q;
  if (V)  delete[ ] V;
  if (pfr) delete pfr;
  if (pdr)  delete pdr;
  if (pstimulus)  delete pstimulus;
}

void Population::init(Istrm& inputf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
  inputf.ignore(200,32); //throwaway line naming population
  double Qinitial;
  inputf.validate("Initial Q",58);
  inputf >> Qinitial;
  for(long i=0; i<nodes; i++)
      Q[i]=Qinitial;
  if (isstimulus) {
    pstimulus = new Timeseries("Stimulus"," of stimulus",inputf);
  } else {
    pfr->init(inputf);
    pdr->init(inputf,ppropagnet,pconnectmat);
    inputf.ignore(200,32); //throwaway space line between populations
  }
}

void Population::dump(ofstream& dumpf){
  dumpf << "Population " << endl; // Write a title line for the population
  dumpf << "Q array : ";
  for(long i=0; i<nodes; i++)
    dumpf << Q[i] <<" ";
  dumpf << endl; // append a endl to end of Q array
  if (isstimulus) {
    pstimulus->dump(dumpf);
  } else {
    pfr->dump(dumpf);
    pdr->dump(dumpf);
  }
}

void Population::restart(Istrm& restartf, PropagNet *ppropagnet, ConnectMat *pconnectmat){
  restartf.ignore(200,32); // Throwaway title line for population
  restartf.ignore(200,58); // Throwaway upto colon i.e. Q array :
  for(long i=0; i<nodes; i++)
    restartf >> Q[i];
  restartf.ignore(200,32); // Throwaway appended endl at end of Q array
  if (isstimulus) {
    pstimulus = new Timeseries("Stimulus"," of stimulus",restartf);
  } else {
    pfr->init(restartf);
    pdr->restart(restartf,ppropagnet,pconnectmat);
  }
}

// Step forward population one timestep method
void Population::stepPop(double timestep){
  if (isstimulus) {
    pstimulus->get(t, Q, nodes);
  } else {
    pdr->stepVa(timestep);
    pdr->SumAfferent(V);
    pfr->getQ(V,Q,nodes,timestep);
  }
  t+=timestep;
}
