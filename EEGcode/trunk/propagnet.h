/***************************************************************************
                          propagnet.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef PROPAGNET_H
#define PROPAGNET_H

#include<fstream>
using std::ofstream;
#include<iostream>

using std::endl;

#include"istrm.h"
#include"qhistorylist.h"
#include"proplist.h"
#include"couplinglist.h"
#include"connectmat.h"
#include"phiout.h"
class ConnectMat; //forward declare ConnectMat. Since we only use pointer to it no need for header
class Poplist; // Forward declaration of Poplist since we only use pointer to it

class PropagNet {
public: 
   PropagNet(double deltat, long nodes, int numpops, int numconct, Istrm& inputf, ofstream& dumpf);
   ~PropagNet();
   void init(Istrm& inputf,Poplist& poplist,ConnectMat& connectmat);
   void dump(ofstream& dumpf);
   void restart(Istrm& restartf,Poplist& poplist,ConnectMat& connectmat);
   void stepQtoP(Poplist& poplist,ConnectMat& connectmat); // Propagate the firing response of each population to pulse densities arriving at the dendrite trees of other populations
   void dumpoutput(ofstream& dumpf);
   void initoutput(Istrm& inputf, ofstream& outputf, int numconct, long nodes);
   void output(ofstream& outputf);
   double **P; // Array storing pointers to the Pab arrays. There are numconct of them
   double **Eta; // Array storing pointers to the Etaab arrays
   int numconnects; //Number of Connections between neural populations
   
private:
   long gridsize;
   const long nodes;
   Qhistorylist *pqhistorylist;
   Proplist *pproplist;
   Couplinglist *pcouplinglist;
   Phiout *pphiout; // Pointer to Phi output object. NULL if not initialized by PropagNet::initoutput
};

#endif
