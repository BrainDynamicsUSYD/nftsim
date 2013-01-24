#include<complex>
#include<fftw3.h>
#include"wave.h"

class WaveFourier : public Wave
{
  WaveFourier(); // no default constructor
  WaveFourier(WaveFourier&); // no copy constructor
  private:
    vector<double> realpart,imagpart;
    std::complex<double> temp_sum;
    std::vector<std::complex<double> > in, out;
    fftw_plan fftplan;
  public:
      WaveFourier( int nodes, double deltat, int index, Population& prepop,Population& postpop, int longside, string topology );
      virtual ~WaveFourier(void);
      void init( Configf& configf );
      void step(void); 
      void output(Output& output) const;
};


