/***************************************************************************
                          stimulus.h  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#ifndef STIMULUS_H
#define STIMULUS_H

#include<fstream>
using std::ifstream;
using std::ofstream;
#include<iostream>
using std::cerr;
using std::endl;
#include<math.h>

#include"random.h"

class Stimulus {
public: 
  Stimulus();
  ~Stimulus();
  void init(ifstream& inputf);
  void dump(ofstream& dumpf);
  void restart(ifstream& restartf);
  void getQstim(float timestep, float *Q, const long nodes);
private:
  float t; // Current time
  int mode; // Number representing mode of stimulus
  float ts; // Time before stimulus onset
  float amp; // Amplitude of stimulus
  float freq; // Frequency of sine stimulus
  float pdur; // Pulse duration of stimulus
  float tperiod; // Period of between pulse repetitions
  void gaussian(float& deviate1, float& deviate2);
  Random random; // Uniform random number generator object
  long idum; // Seed for random number generator
};

#endif
