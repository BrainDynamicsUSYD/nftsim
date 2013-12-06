#ifdef WAVEFOURIER

#include<cmath>
#include"wavefourier.h"

WaveFourier::WaveFourier( int nodes, double deltat, int index,
      Population& prepop, Population& postpop, int longside, string topology )
    : Wave( nodes, deltat, index, prepop, postpop, longside, topology )
{
}

void WaveFourier::init(Configf& configf)
{
  Wave::init(configf);
  for(int i = 0; i<nodes;i++) {
    in.push_back(0);
    out.push_back(0);
  }

  fftplan = fftw_plan_dft_2d( sqrt(nodes), sqrt(nodes),
    reinterpret_cast<fftw_complex*>(&in[0]),
    reinterpret_cast<fftw_complex*>(&out[0]),
    FFTW_FORWARD, FFTW_MEASURE );
  realpart.push_back(0);
  imagpart.push_back(0);
}

void WaveFourier::step(void)
{
  Wave::step();
  //new stuff
  // DO FFT AND SUM
  for(int i=0; i<nodes; i++)
    in[i] = p[i];
    
  fftw_execute(fftplan);
  
  temp_sum = 0;    
  for(int i=0; i<nodes; i++)
    temp_sum += out[i];

  realpart[0] = std::real(temp_sum)/nodes;
  imagpart[0] = std::imag(temp_sum)/nodes;
}


void WaveFourier::output(Output& output) const
{
  output.prefix("WaveFourier",index+1);
  output.singleNode("Real",realpart);
  output.singleNode("Imag",imagpart);
}


WaveFourier::~WaveFourier(void)
{
  fftw_destroy_plan(fftplan);
}

#endif
