/***************************************************************************
                          conc.h  -  
                             -------------------
    copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef CONC_H
#define CONC_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"random.h"

class Concentration {
public: 
  Concentration();
  ~Concentration();
  float init(Istrm& inputf);
  void dump(ofstream& dumpf);
  float restart(Istrm& restartf);
  void get(double timestep, double *Q, const long nodes);
private:
  double t; // Current time
  int mode; // Number representing mode of stimulus
  double ts; // Time before stimulus onset
  double tpeak; // Time to stimulus peak
  double amp; // Amplitude of stimulus
  double freq; // Frequency of sine stimulus
  double pdur; // Pulse duration of stimulus
  double tperiod; // Period of between pulse repetitions
  double mean; //Mean firing rate of stimulus
 double deltax; //Grid spacing for stimulus
  double xcent; //x coordinate of Gaussian centre
  double ycent; //y coordinate of Gaussian centre
  double xspread; //Spread of Gaussian in x direction
 double yspread; //Spread of Gaussian in y direction
  Random random; //Random number generator object
};

#endif
