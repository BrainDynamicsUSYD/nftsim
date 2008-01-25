/***************************************************************************
                          modcouple.cpp  - 
                             -------------------
   copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include <math.h>
#include<string>
using std::string;
#include<sstream>
using std::stringstream;
#include "modcouple.h"

Modcouple::Modcouple(long numnodes, double deltat)
  :nodes(numnodes),timestep(deltat){
  conc = new double[nodes];
  previousconc = new double[nodes];
  h = new double[nodes];
  dhdt = new double[nodes];
  nu = new double[nodes];
}

Modcouple::~Modcouple(){
  delete [ ] conc;
  delete [ ] previousconc;
  delete [ ] h;
  delete [ ] dhdt;
  delete [ ] nu;
  if(nodeoftrace) delete [ ] nodeoftrace;
  if(pconcobj) delete pconcobj;
}

void Modcouple::init(Istrm& inputf, int coupleid){
  inputf.validate("Nuzero",58);
  inputf >> nuzero;
  inputf.validate("Nuscal",58);
  inputf >> nuscal;
  inputf.validate("Mu",58);
  inputf >> mu;
  inputf.validate("Lambda",58);
  inputf >> lambda;
  inputf.validate("k",58);
  inputf >> k;
  float mean;
  pconcobj = new Concentration();
  mean=pconcobj->init(inputf);
  for(int i=0; i<nodes; i++){
    previousconc[i]=mean;
    h[i]=mean;
    dhdt[i]=0.0;
  }
  initoutput(inputf,coupleid);
}

void Modcouple::dump(ofstream& dumpf){
  dumpf << "Nuzero: " << nuzero << " ";
  dumpf << "Nuscal: " << nuscal << " ";
  dumpf << "Mu: " << mu << " ";
  dumpf << "Lambda: " << lambda << " ";
  dumpf << "k: " << k << " ";
  pconcobj->dump(dumpf);
}

void Modcouple::restart(Istrm& restartf, int coupleid){
  restartf.validate("Nuzero",58);
  restartf >> nuzero;
  restartf.validate("Nuscal",58);
  restartf >> nuscal;
  restartf.validate("Mu",58);
  restartf >> mu;
  restartf.validate("Lambda",58);
  restartf >> lambda;
  restartf.validate("k",58);
  restartf >> k;
  float mean;
  pconcobj = new Concentration();
  mean=pconcobj->restart(restartf);
  for(int i=0; i<nodes; i++){
    previousconc[i]=mean;
    h[i]=mean;
    dhdt[i]=0.0;
  }
  initoutput(restartf,coupleid);
}

void Modcouple::initoutput(Istrm& inputf, int coupleid){
  coupleid++; // (coupleid+1) is to ensure numbering from one not zero in output
  stringstream ss(stringstream::in | stringstream::out);
  ss << "eegcode.synaptout." << coupleid;
  outputf.open(ss.str().c_str(),ios::out);
  if( !outputf ){
    cerr << "Unable to open 'eegcode.synaptout." << coupleid << "' for output \n";
    exit(EXIT_FAILURE);
  }
  int optionnum;
  inputf.validate("traces",58); 
  inputf >> numtraces;
  outputf << "Output Synapse - Number of traces: " << numtraces << " ";
  nodeoftrace = new long [numtraces];
  long i=0;
  while(i<numtraces){
    long temp1;
    inputf.validate("Single/All",58); 
    optionnum=inputf.choose("Single:1 All:2 ",32);
    outputf << "Single/All nodes:";
    if(1==optionnum){
      outputf << " Single ";
      inputf.ignore(200,58); // throwaway everything up to colon character
      inputf >> temp1;
      if(temp1>nodes || temp1<1){
        cerr << "Node number " << temp1 << " requested for output is invalid" << endl;
        exit(EXIT_FAILURE);
      }     
      nodeoftrace[i]=temp1-1; //'-1' as nodeoftrace runs [0:n-1] rather than [1:n]
      outputf << "Node Number : "<< temp1 << endl;
    }
    if(2==optionnum){
      outputf << " All nodes";
      inputf.ignore(200,32); // throwaway everything up to colon character
      nodeoftrace[i]=0; // Request the first node be outputted
      for(long j=1;j<nodes;j++){ // Request the remaining nodes be outputted
	nodeoftrace[i+j]=j;
      }
      i+=nodes-1; // increment by number of traces now requested (except the one in outer loop)
      if( i>(numtraces-1) ){
        cerr << "Not enough traces specified to output all - please increase" << endl;
	exit(EXIT_FAILURE);
      }
    }
    i++;
  }
}

void Modcouple::output(){
  for(long i=0; i<numtraces;i++){
    outputf << setprecision(14) << *(nu+nodeoftrace[i]) << endl; // Add a field with value at Nu point requested
  }
  outputf << endl; // Insert a blank line at the end of the record
}

//
// Sum the coupling terms
//
void Modcouple::updatePa(double *Pa, double *Etaa){
//
// Steps h(t+Timestep) using current conc(t) and current h(t)
//
//  The program assumes that mu, lambda are constant and conc(t) is
//  linear for the time step
//  Under these assumptions the solution can be explicitly obtained.
//  Calculating the explicit solution is computationally faster than using
//  Runge-Kutta to evaluate the time step.
//  At current time mu and lambda are not functions of space and so 
//  computed variables are used to speed up the calculation.

  double adjustedconc;
  double muminuslambda;
  double factormulambda;
  double deltaconcdeltat;
  double C1;
  double C1expmu;
  double C2explambda;
  
  pconcobj->get(timestep, conc, nodes);
  expmu=exp(-mu*timestep);
  explambda=exp(-lambda*timestep);
  muminuslambda=mu-lambda;
  factormulambda=(1.0/mu)+(1.0/lambda);
  for(int i=0; i<nodes; i++){
//
    deltaconcdeltat=(conc[i]-previousconc[i])/timestep;
    adjustedconc=previousconc[i]-factormulambda*deltaconcdeltat-h[i];
    C1=(adjustedconc*lambda-dhdt[i]+deltaconcdeltat)/muminuslambda;
    C1expmu=C1*expmu;
    C2explambda=explambda*(-C1-adjustedconc);
    h[i]=C1expmu+C2explambda+conc[i]-factormulambda*deltaconcdeltat;
    dhdt[i]=C1expmu*(-mu)+C2explambda*(-lambda)+deltaconcdeltat;
    previousconc[i]=conc[i]; //Save neurotransmitter concentration for next step
//    
    nu[i]=nuzero*((1-nuscal)*exp(-h[i]/k) + nuscal);
    Pa[i]=nu[i]*Etaa[i];
    }
}
