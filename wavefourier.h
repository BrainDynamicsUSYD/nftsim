#ifdef WAVEFOURIER
#ifndef WAVEFOURIER_H
#define WAVEFOUERIER_H

#include<complex>
using std::complex;
#include<vector>
using std::vector;
#include<fftw3.h>
#include"wave.h"

class WaveFourier : public Wave
{
  WaveFourier(); // no default constructor
  WaveFourier(WaveFourier&); // no copy constructor
protected:
  vector<double> realpart,imagpart;
  complex<double> temp_sum;
  vector<std::complex<double> > in, out;
  fftw_plan fftplan;
public:
  WaveFourier( int nodes, double deltat, int index,
    Population& prepop,Population& postpop, int longside, string topology );
  virtual ~WaveFourier(void);
  void init( Configf& configf );
  void step(void); 
  void output(Output& output) const;
};

#endif
#endif
