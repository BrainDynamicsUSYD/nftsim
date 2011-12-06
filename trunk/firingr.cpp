/***************************************************************************
                          firingr.cpp  -  Compute firing response sigmoid
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "firingr.h"
#include<math.h>
using std::endl;

FiringR::FiringR(int popindex,Istrm& inputf):pindex(popindex){
  double initval = 0;
  inputf.validate("Firin",103);
  inputf.validate("respo",110); 
  inputf.validate("s",101); // Read succesively upto the end of "Firing response"
  type=inputf.choose("Sigmoid:1 Theta:2 Linear:3",':');
  switch(type) {
    case 1:
      inputf.validate("Theta",':'); // read up to "Theta:"
    case 2:
      inputf >> initval;
      params.push_back(Parameter("Theta",initval));
      inputf.validate("Sigma",':');
      inputf >> initval;
      params.push_back(Parameter("Sigma",initval));
      inputf.validate("Qmax",':');
      inputf >> initval;
      params.push_back(Parameter("Qmax",initval));
      break;
    case 3:
      inputf.validate("Gradient",':');
      inputf >> initval;
      params.push_back(Parameter("Gradient",initval));
      inputf.validate("Intercept",':');
      inputf >> initval;
      params.push_back(Parameter("Intercept",initval));
      break;
    default:
      std::cerr<<"Unrecognized firing response."<<endl;
      exit(EXIT_FAILURE);
  }
  inputf.ignore(200,' '); // Ignore appended endline at end of firing response
}

FiringR::~FiringR(){
}
//
// Method to transform V into Q via sigmoid firing response
//
void FiringR::getQ(double *V,double *Q,long nodes,double timestep){
  switch(type) {
    case 1:
    case 2:
      for(long i=0; i<nodes; i++)
        Q[i] = params[2].get()/(1.0F+exp(-(V[i]-params[0].get())/params[1].get()));
      break;
    case 3:
      for(long i=0; i<nodes; i++)
        Q[i] = V[i]*params[0].get() +params[1].get();
      break;
  }
  
}

void FiringR::dump(ofstream& dumpf){
  dumpf << "Firing response "; // Insert Firing Response title
  switch(type) {
    case 1:
    case 2:
      dumpf << "Sigmoid: "; break;
    case 3:
      dumpf << "Linear: "; break;
  }
  for(int i=0; i<params.size(); i++ )
    params[i].dump(dumpf);
  dumpf << endl; //Append endl at end of firing response figures
}

