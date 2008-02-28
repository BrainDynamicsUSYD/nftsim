/***************************************************************************
                          propagnet.cpp  -  Network of propagator
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<math.h>
#include "propagnet.h"

PropagNet::PropagNet(double deltat, long totalnodes, int numpops, int numconct, Istrm& inputf, ofstream& dumpf)
                      :numconnects(numconct),nodes(totalnodes){
  P = new double *[numconnects];
  for(int i=0;i<numconnects;i++)
    P[i]= new double[nodes]; 
  Eta = new double *[numconnects];
  for(int i=0;i<numconnects;i++)
    Eta[i]= new double[nodes]; 
  pqhistorylist = new Qhistorylist(inputf,dumpf,numpops,nodes);
  pproplist = new Proplist(inputf,dumpf,numconct,nodes,deltat);
  pcouplinglist = new Couplinglist(inputf,dumpf,numconct,nodes,deltat);
}

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

void PropagNet::init(Istrm& inputf, Poplist *ppoplist, ConnectMat *pconnectmat){
  inputf.ignore(200,32); // Throwaway blank line
  inputf.ignore(200,32); // Throwaway title line of propagation data
  pqhistorylist->init(inputf, ppoplist);
  pproplist->init(inputf,pqhistorylist,pconnectmat);
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
  pcouplinglist->updateP(P, Eta); // Weight signal strengths for links between neural populations
}

void PropagNet::initoutput(Istrm& inputf, ofstream& outputf, int numconct, long totalnodes){
  pphiout = new Phiout(inputf, outputf, numconct, totalnodes);
}

void PropagNet::dumpoutput(ofstream& dumpf){
  pphiout->dump(dumpf);
}

void PropagNet::output(ofstream& outputf){
  pphiout->output(outputf, Eta);
  pcouplinglist->output();
}
