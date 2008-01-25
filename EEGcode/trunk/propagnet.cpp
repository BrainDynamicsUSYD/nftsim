/***************************************************************************
                          propagnet.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<math.h>
#include "propagnet.h"

//
// Constructor for Propagnet
//
PropagNet::PropagNet(float deltat, long totalnodes, int numpops, int numconct, Istrm& inputf, ofstream& dumpf)
                      :numconnects(numconct),nodes(totalnodes){
  gridsize=static_cast<long>((sqrt(nodes)+2)*(sqrt(nodes)+2));
  if (sqrt( static_cast<float>(nodes)) != floor(sqrt( static_cast<float>(nodes)))){
    cerr << "Wave equation solver assumes square grid. Nodes per population must be a perfect square number" << endl;
    exit(EXIT_FAILURE);
  }
  P = new float *[numconnects];
  for(int i=0;i<numconnects;i++)
    P[i]= new float[nodes]; 
  Eta = new float *[numconnects];
  for(int i=0;i<numconnects;i++)
    Eta[i]= new float[nodes]; 
  pqhistorylist = new Qhistorylist(inputf,dumpf,numpops,gridsize);
  pproplist = new Proplist(inputf,dumpf,numconct,gridsize,deltat);
  pcouplinglist = new Couplinglist(numconct);
}

//
// Destructor for Propagnet
//
PropagNet::~PropagNet(){
  for(int i=0;i<numconnects;i++){
    delete [ ] P[i];
  }
  delete [ ] P;
  for(int i=0;i<numconnects;i++){
    delete [ ] Eta[i];
  }
  delete [ ] Eta;
  delete pqhistorylist;
  delete pproplist;
  delete pcouplinglist;
//
//
  if (pphiout) delete pphiout; // Free Phiout object if it was initialized by PropagNet::initoutput()
}

void PropagNet::init(Istrm& inputf, Poplist *ppoplist){
  inputf.ignore(200,32); // Throwaway blank line
  inputf.ignore(200,32); // Throwaway title line of propagation data
  pqhistorylist->init(inputf, ppoplist);
  pproplist->init(inputf);
  pcouplinglist->init(inputf);
}

void PropagNet::dump(ofstream& dumpf){
  dumpf << "Propagation data" << endl;
  pqhistorylist->dump(dumpf);
  pproplist->dump(dumpf);
  pcouplinglist->dump(dumpf);
}

//
void PropagNet::restart(Istrm& restartf, Poplist *ppoplist){
  restartf.ignore(200,32); // Throwaway blank line
  pqhistorylist->restart(restartf, ppoplist);
  pproplist->restart(restartf);
  pcouplinglist->restart(restartf);
}

// Propagate the firing response of each population to pulse densities arriving at the dendrite trees of other
//
void PropagNet::stepQtoP(Poplist * ppoplist, ConnectMat * pconnectmat){
  pqhistorylist->updateQhistories(ppoplist); // Get new Q histories and add them to the keyrings
  pproplist->step(Eta, pqhistorylist, pconnectmat); // Transform Q to Eta via stepping forward multiple wave equations
  pcouplinglist->updateP(P, Eta, nodes); // Weight signal strengths for links between neural populations
}

void PropagNet::initoutput(Istrm& inputf, ofstream& outputf){
  pphiout = new Phiout(inputf, outputf);
}

void PropagNet::output(ofstream& outputf){
  pphiout->output(outputf, Eta);
}
