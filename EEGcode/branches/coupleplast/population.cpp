/***************************************************************************
                          population.cpp  -  Population object describes a
			                     neural population
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "population.h"
#include "connectmat.h"
using std::endl;

Population::Population(long n, int popindex,ConnectMat& connectmat)
             :t(0),pindex(popindex),V(0),nodes(n),pfr(0),pdr(0),pstimulus(0){
  Q = new double[nodes];
  isstimulus=true;
  if (connectmat.getDRlength(popindex)) { //If populations are attached to this dendritic tree it is not a stimulus population
    isstimulus=false;
    V = new double[nodes];
    pdr = new DendriticRlist(nodes,popindex,connectmat);
  }
}

Population::~Population(){
  delete[ ] Q;
  if (V)  delete[ ] V;
  if (pfr) delete pfr;
  if (pdr)  delete pdr;
  if (pstimulus)  delete pstimulus;
}

void Population::init(Istrm& inputf,PropagNet& propagnet,ConnectMat& connectmat){
  inputf.ignore(200,32); //throwaway line naming population
  double Qinitial;
  inputf.validate("Initial Q",58);
  inputf >> Qinitial;
  for(long i=0; i<nodes; i++)
      Q[i]=Qinitial;
  if (isstimulus) {
    pstimulus = new Timeseries("Stimulus"," of stimulus",inputf);
  } else {
    pfr = new FiringR(pindex,inputf);
    pdr->init(inputf,propagnet,connectmat);
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

void Population::restart(Istrm& restartf,PropagNet& propagnet,ConnectMat& connectmat){
  restartf.ignore(200,32); // Throwaway title line for population
  restartf.ignore(200,58); // Throwaway upto colon i.e. Q array :
  for(long i=0; i<nodes; i++)
    restartf >> Q[i];
  restartf.ignore(200,32); // Throwaway appended endl at end of Q array
  if (isstimulus) {
    pstimulus = new Timeseries("Stimulus"," of stimulus",restartf);
  } else {
    pfr = new FiringR(pindex,restartf);
    pdr->restart(restartf,propagnet,connectmat);
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
