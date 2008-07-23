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
#include<string>
using std::string;

#include"istrm.h"
#include"random.h"

class Timeseries {
public: 
  Timeseries(const char * typeid1, const char * typeid2);
  ~Timeseries();
  float init(Istrm& inputf);
  void dump(ofstream& dumpf);
  float restart(Istrm& restartf);
  void get(double t, double *tseries, const long nodes);
private:
  string id1; // name of type of time series used in mode e.g. "Concentration" .. modes
  string id2; // name of type of time series used in start time messsage
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
  double stepheight; // Step height in JC's ramped input
  double stepwidth; // Step width in JC's ramped input
  Random random; //Random number generator object
};

#endif
