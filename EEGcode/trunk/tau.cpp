/***************************************************************************
                          tau.cpp  -  time delay object
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include<cstdlib>
#include "tau.h"
#include "qhistory.h"
using std::endl;

Tau::Tau(long nodes,double dt,Istrm& inputf,Qhistory& qhistory):
            nodes(nodes),deltat(dt){
  int optionnum;
  int taumax=0; // maximum timesteps amongst the tau values
  optionnum=inputf.choose("Tauab:1 Tauabt:2 TauabArray:3 TauabtArray:4",58);
  float tauabfloat;
  isarraytau=false; //default to false
  if(1==optionnum||2==optionnum){
    inputf >> tauabfloat;
    if(2==optionnum){tauabfloat=tauabfloat/deltat;} // scale tau:sec to steps
    tauab=int(tauabfloat);
    taumax=tauab;
  }
  if(3==optionnum||4==optionnum){
    isarraytau=true;
    tauarr = new long[nodes];
    qarray = new double[nodes];
// Work with first value separately
    inputf >> tauabfloat;
    if(2==optionnum){tauabfloat=tauabfloat/deltat;}//scale tau:sec to steps
    tauarr[0]=int(tauabfloat);
    for(long i=1;i<nodes;i++){
      inputf.ignore(200,58); // throw away everything uptil colon character
      inputf >> tauabfloat;
      if(2==optionnum){tauabfloat=tauabfloat/deltat;}//scale tau:sec to steps
      tauarr[i]=int(tauabfloat);
      if(tauarr[i]>taumax){
        taumax=tauarr[i];
      }
    }
  }
  if( (1>optionnum)||(4<optionnum) ){
    std::cerr << "Last read looking for Tauab, Tauabt, TauabArray, or";
    std::cerr << " TauabtArray found none of them" << endl;
    exit(EXIT_FAILURE);
  }
  if(tauabfloat<1 && tauabfloat>0){
    std::cerr << "Last read Tauab: " << tauabfloat << endl;
    std::cerr << "Tauab must be greater than 1 as it is measured in" << endl;
    std::cerr << "time steps not a time measured in seconds" << endl;
    exit(EXIT_FAILURE);
  }
  qhistory.grow(taumax);
}

Tau::~Tau(){
  if(isarraytau){
    delete [] tauarr;
    delete [] qarray;
  }
}

void Tau::dump(std::ofstream& dumpf){
  if(isarraytau){
    dumpf << "- TauabArray";
    for(long i=0;i<nodes;i++){
      dumpf << ": " << tauarr[i] << " ";
    }
  } else {
    dumpf << "- Tauab: " << tauab << " ";
  }
};
