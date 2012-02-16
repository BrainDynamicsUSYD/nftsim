/***************************************************************************
                          timeseries.h  - a generic time series generator 
                             -------------------
    copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef TIMESERIES_H
#define TIMESERIES_H

#include<fstream>
#include<iostream>
#include<string>
using std::string;
#include<vector>
using std::vector;
#include"configf.h"
#include"random.h"

class Timeseries {
public: 
  Timeseries( Configf& inputf );
  ~Timeseries();
  void dump(std::ofstream& dumpf);
  void get(double t, double *tseries, const long nodes);
private:
  Timeseries(Timeseries& ) ; // No copy contructor

  string mode; // Number representing mode of timeseries
  double ts; // Time before timeseries onset
  Random *random; //Pointer to Random number generator object
  std::vector<Timeseries*> sarray; // Array of stimuli when mode==0

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
  long seed; // seed for random number generator
};

#endif
