#ifndef TIMESERIES_H
#define TIMESERIES_H

#include<string>
using std::string;
#include<vector>
using std::vector;
#include"random.h"
#include"nf.h"

class Timeseries : public NF
{
  Timeseries(Timeseries& ) ; // No copy contructor
  Timeseries(void) ; // No default contructor

  string mode; // Number representing mode of timeseries
  double t; // time in seconds
  double tend; // time to end stimulus
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
  int seed; // seed for random number generator

protected:
  void init( Configf& configf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;
public: 
  Timeseries( int nodes, double deltat, int index );
  virtual ~Timeseries();
  void step(void);
  void fire( vector<double>& Q ) const;
};

#endif
