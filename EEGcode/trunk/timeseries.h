/***************************************************************************
                          timeseries.h  - a generic time series generator 
                             -------------------
    copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef TIMESERIES_H
#define TIMESERIES_H

#include<fstream>
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;

#include"istrm.h"
#include"random.h"

class Timeseries {
public: 
  Timeseries();
  ~Timeseries();
  float init(Istrm& inputf);
  void dump(ofstream& dumpf);
  float restart(Istrm& restartf);
  void get(double t, double *tseries, const long nodes);
private:
  int mode; // Number representing mode of timeseries
  double ts; // Time before timeseries onset
  double tpeak; // Time to peak
  double amp; // Amplitude of timeseries
  double freq; // Frequency of sine timeseries
  double pdur; // Pulse duration of timeseries
  double tperiod; // Period of between pulse repetitions
  double mean; //Mean timeseries
 double deltax; //Grid spacing for 2D timeseries
  double xcent; //x coordinate of Gaussian centre
  double ycent; //y coordinate of Gaussian centre
  double xspread; //Spread of Gaussian in x direction
 double yspread; //Spread of Gaussian in y direction
  Random random; //Random number generator object
};

#endif
