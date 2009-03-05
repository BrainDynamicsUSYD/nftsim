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
  :t(0),nodes(numnodes),timestep(deltat){
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
  if(synnodes) delete [ ] synnodes;
  if(concnodes) delete [ ] concnodes;
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
  pconcobj = new Timeseries("Concentration"," of Concentration");
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
  pconcobj = new Timeseries("Concentration"," of Concentration");
  mean=pconcobj->init(restartf);
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
  synapoutf.open(ss.str().c_str(),ios::out);
  if( !synapoutf ){
    cerr << "Unable to open 'eegcode.synaptout." << coupleid << "' for output \n";
    exit(EXIT_FAILURE);
  }
  int optionnum;
  inputf.validate("traces",58); 
  inputf >> synaptraces;
  synapoutf << "Output Synapse - Number of traces: " << synaptraces << " ";
  synnodes = new long [synaptraces];
  long i=0;
  while(i<synaptraces){
    long temp1;
    inputf.validate("Single/All",58); 
    optionnum=inputf.choose("Single:1 All:2 ",32);
    synapoutf << "Single/All nodes:";
    if(1==optionnum){
      synapoutf << " Single ";
      inputf.ignore(200,58); // throwaway everything up to colon character
      inputf >> temp1;
      if(temp1>nodes || temp1<1){
        cerr << "Synaptic node number " << temp1 << " requested for output is invalid" << endl;
        exit(EXIT_FAILURE);
      }     
      synnodes[i]=temp1-1; //'-1' as nodeoftrace runs [0:n-1] rather than [1:n]
      synapoutf << "Node Number : "<< temp1 << endl;
    }
    if(2==optionnum){
      synapoutf << " All nodes";
      inputf.ignore(200,32); // throwaway everything up to colon character
      synnodes[i]=0; // Request the first node be outputted
      for(long j=1;j<nodes;j++){ // Request the remaining nodes be outputted
	    synnodes[i+j]=j;
      }
      i+=nodes-1; // increment by number of traces now requested (except the one in outer loop)
      if( i>(synaptraces-1) ){
        cerr << "Not enough traces specified to output all - please increase" << endl;
	exit(EXIT_FAILURE);
      }
    }
    i++;
  }
  stringstream ss1(stringstream::in | stringstream::out);
  ss1 << "eegcode.concout." << coupleid;
  concoutf.open(ss1.str().c_str(),ios::out);
  if( !concoutf ){
    cerr << "Unable to open 'eegcode.concout." << coupleid << "' for output \n";
    exit(EXIT_FAILURE);
  }
  inputf.validate("conc.traces",58);
  inputf >> conctraces;
  concoutf << "Output Concentration - Number of traces: " << conctraces << " ";
  concnodes = new long [conctraces];
  i=0;
  while(i<conctraces){
    long temp1;
    inputf.validate("Single/All",58); 
    optionnum=inputf.choose("Single:1 All:2 ",32);
    concoutf << "Single/All nodes:";
    if(1==optionnum){
                     
      concoutf << " Single ";
      inputf.ignore(200,58); // throwaway everything up to colon character
      inputf >> temp1;
      if(temp1>nodes || temp1<1){
        cerr << "Concentration node number " << temp1 << " requested for output is invalid" << endl;
        exit(EXIT_FAILURE);
      }     
      concnodes[i]=temp1-1; //'-1' as nodeoftrace runs [0:n-1] rather than [1:n]
      concoutf << "Node Number : "<< temp1 << endl;
    }
    if(2==optionnum){
      concoutf << " All nodes";
      inputf.ignore(200,32); // throwaway everything up to colon character
      concnodes[i]=0; // Request the first node be outputted
      for(long j=1;j<nodes;j++){ // Request the remaining nodes be outputted
	    concnodes[i+j]=j;
      }
      i+=nodes-1; // increment by number of traces now requested (except the one in outer loop)
      if( i>(conctraces-1) ){
        cerr << "Not enough traces specified to output all - please increase" << endl;
	exit(EXIT_FAILURE);
      }
    }
    i++;
  }
}

void Modcouple::output(){
  for(long i=0; i<synaptraces;i++){
    synapoutf << setprecision(14) << *(nu+synnodes[i]) << endl; // Add a field with value at Nu point requested
  }
  synapoutf << endl; // Insert a blank line at the end of the record
  for(long i=0; i<conctraces;i++){
    concoutf << setprecision(14) << *(conc+concnodes[i]) << endl; // Add a field with value at Nu point requested
  }
  concoutf << endl; // Insert a blank line at the end of the record
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
  double factormulambda;
  double deltaconcdeltat;
  double C1;
    
  pconcobj->get(t, conc, nodes);
  expmu=exp(-mu*timestep);
  factormulambda=(1.0/mu)+(1.0/lambda);
  if(mu!=lambda){
    double muminuslambda;
    double C1expmu;
    double C2explambda;
    explambda=exp(-lambda*timestep);
    muminuslambda=mu-lambda;
    for(long i=0; i<nodes; i++){
//
      deltaconcdeltat=(conc[i]-previousconc[i])/timestep;
      adjustedconc=previousconc[i]-factormulambda*deltaconcdeltat-h[i];
      C1=(adjustedconc*lambda-dhdt[i]+deltaconcdeltat)/muminuslambda;
      C1expmu=C1*expmu;
      C2explambda=explambda*(-C1-adjustedconc);
      h[i]=C1expmu+C2explambda+conc[i]-factormulambda*deltaconcdeltat;
      dhdt[i]=C1expmu*(-mu)+C2explambda*(-lambda)+deltaconcdeltat;
      previousconc[i]=conc[i]; //Save neurotransmitter concentration for next step
    }
  }
  else{
    double C1deltatplusc2;
    for(long i=0; i<nodes; i++){
      deltaconcdeltat=(conc[i]-previousconc[i])/timestep;
      adjustedconc=previousconc[i]-factormulambda*deltaconcdeltat-h[i];
      C1=dhdt[i]-mu*adjustedconc-deltaconcdeltat;
      C1deltatplusc2=C1*timestep+adjustedconc;
      h[i]=C1deltatplusc2*expmu+conc[i]-factormulambda*deltaconcdeltat;
      dhdt[i]=(C1-mu*C1deltatplusc2)*expmu+deltaconcdeltat;
      previousconc[i]=conc[i]; // Save current conc for next step
    }
  }
//
  t+=timestep;   
  for(long i=0; i<nodes; i++){
    nu[i]=nuzero*((1-nuscal)*exp(-h[i]/k) + nuscal);
    Pa[i]=nu[i]*Etaa[i];
  }
}
