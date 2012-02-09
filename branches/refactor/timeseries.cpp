/***************************************************************************
                          timeseries.cpp  -  a generic time series generator 
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include <cmath>
#include<iostream>
using std::endl;
#include "timeseries.h"
#include"istrm.h"

Timeseries::Timeseries(Istrm& inputf)
{
  random=0; // Set pointer to null in case random is not created
  seed=-98716872;
  inputf.Param("Mode",mode);
  inputf.Param("Onset",ts);
  if( mode=="Composite" ) {
    // take in stimulus transition times and specifications of each stimulus
    int nStim; inputf.Param("Stimuli",nStim);
    for( int i=0; i<nStim; i++ )
      sarray.push_back( new Timeseries(inputf) );
  }
  else if( mode=="Const" ) { // constant noise
    inputf.Param("Mean",mean);
  }
  else if( mode=="White" ) { // white noise
    inputf.Optional("Ranseed",seed);
    inputf.Param("Amplitude",amp);
    random = new Random(seed);
    inputf.Param("Mean",mean);
  }
  else if( mode=="CoherentWhite" ) { // spatially homogeneous white noise
    inputf.Optional("Ranseed",seed);
    inputf.Param("Amplitude",amp);
    random = new Random(seed);
    inputf.Param("Mean",mean);
  }
  else if( mode=="Pulse" ) { // periodic pulse pattern
    inputf.Param("Amplitude",amp);
    inputf.Param("Pulse width",pdur);
    if( !inputf.Optional("Repetition",tperiod) )
      tperiod = -1; // if repetition period not specified, do just one pulse
  }
  else if( mode=="Sine" ) { // sinusoidal stimuli
    inputf.Param("Amplitude",amp);
    inputf.Param("Frequency",freq);
  }
  else if( mode=="Gaussian" ) { // spatial and temporal gaussian
    inputf.Param("Amplitude",amp);
    inputf.Param("Time to peak of stimulus",tpeak);
    inputf.Param("Pulse Duration",pdur);
    inputf.Param("Grid Spacing",deltax);
    inputf.Param("x location",xcent);
    inputf.Param("y location",ycent);
    inputf.Param("x spread",xspread);
    inputf.Param("y spread",yspread);
  }
  else if( mode=="Ramp" ) { // ramp concentration pattern
    inputf.Param("Step height",stepheight);
    inputf.Param("Step width",stepwidth);
  }
  else if( mode=="GaussPulse" ) { // gaussian pulse
    inputf.Param("Amplitude",amp);
    inputf.Param("Pulse Duration",pdur);
    inputf.Param("Time at peak",tpeak);
  }
}

Timeseries::~Timeseries(){
  if(random) delete random;
  for( unsigned int i=0; i<sarray.size(); i++ )
    if(sarray[i]) delete sarray[i];
}

void Timeseries::dump(std::ofstream& dumpf){
  dumpf << " Mode:" << mode << " ";
  dumpf << "Onset: " << ts << " ";
  if( mode=="Composite" ) {
    dumpf << "Stimuli: " << sarray.size() << endl;
  }
  else if( mode=="Const" ) { // constant noise
    dumpf << "Mean" << mean <<endl;
  }
  else if( mode=="White" ) { // white noise
    dumpf << "Ranseed: " << seed;
    dumpf << " Amplitude: " << amp;
    dumpf << " Mean: " << mean << endl;
  }
  else if( mode=="CoherentWhite" ) { // spatially homogeneous white noise
    dumpf << "Ranseed: " << seed;
    dumpf << " Amplitude: " << amp;
    dumpf << " Mean: " << mean << endl;
  }
  else if( mode=="Pulse" ) { // periodic pulse pattern
    dumpf << "Amplitude: " << amp;
    dumpf << " Pulse width: " << pdur;
    dumpf << " Repetition: " << tperiod << endl;
  }
  else if( mode=="Sine" ) { // sinusoidal stimuli
    dumpf << "Amplitude: " << amp;
    dumpf << " Frequency: " << freq << endl;
  }
  else if( mode=="Gaussian" ) { // spatial and temporal gaussian
    dumpf << "Amplitude: " << amp;
    dumpf << " Pulse Duration: " << pdur;
    dumpf << " Grid Spacing: " << deltax;
    dumpf << " x location: " << xcent;
    dumpf << " y location: " << ycent;
    dumpf << " x spread: " << xspread;
    dumpf << " y spread: " << yspread << endl;
  }
  else if( mode=="Ramp" ) { // ramp concentration pattern
    dumpf << "Step height: " << stepheight;
    dumpf << " Step width: " << stepwidth << endl;
  }
  else if( mode=="GaussPulse" ) { // gaussian pulse
    dumpf << "Amplitude: " << amp;
    dumpf << " Pulse Duration: " << pdur;
    dumpf << " Time at peak: " << tpeak << endl;
  }
}

void Timeseries::get(double t, double *tseries, const long nodes)
{
if( t<ts ) return;
  if( mode == "Composite" ) {
    for( int i=0; i<nodes; i++ )
      tseries[i] = 0; // reset firing, then let each stimulus superimpose
    for( unsigned int i=0; i<sarray.size(); i++ )
      sarray[i]->get( t, tseries, nodes );
  }
  else if( mode=="Const" ) { // constant noise
    for( unsigned int i=0; i<sarray.size(); i++ )
      tseries[i] += mean;
  }
  else if( mode=="White" ) { // white noise
    // For efficiency reasons random.gaussian random deviates are usually
    // calculated in pairs. This is the reason for slightly more complex
    // updating routine here
    double deviate1, deviate2;
    // if nodes is even then update every point in tseries[]
    // if nodes is odd then update all but the last point in tseries[]
    for( long i=0; i<nodes-1; i+=2 ){
      random->gaussian(deviate1,deviate2);	
      tseries[i]   += amp*deviate1 + mean;
      tseries[i+1] += amp*deviate2 + mean;
    }
    // if nodes is odd update last point which was otherwise not updated above
    if(nodes%2){
      random->gaussian(deviate1,deviate2);
      tseries[nodes] += amp*deviate1 + mean;
    }
  }
  else if( mode=="CoherentWhite" ) { // spatially homogeneous white noise
    double deviate1, deviate2;
    random->gaussian(deviate1,deviate2);
    for( long i=0; i<nodes; i++ )
      tseries[i]=amp*deviate1 + mean;
  }
  else if( mode=="Pulse" ) { // periodic pulse pattern
    if( fmod((t-ts),tperiod)<pdur )
      for(long i=0; i<nodes; i++)
        tseries[i] += amp;
  }
  else if( mode=="Sine" ) { // sinusoidal stimuli
    for(long i=0; i<nodes; i++)
      tseries[i]=amp*sin(6.2831853F*freq*t);
  }
  else if( mode=="Gaussian" ) { // spatial and temporal gaussian
    float x = 0;
    float y = 0;
    int ij = 0;
    int size = 0;
    float arg = 0;	
    float temporal = 0;
    temporal = (1 / (sqrt(6.2831853)*pdur)) * 
      (exp(-0.5*pow((t-tpeak), 2) / (pdur*pdur))); //3.1415926F
    size = (int) sqrt(nodes);
    // Do spatial Gausian here....
    for(long i=0; i<size; i++){
      for(long j=0; j<size; j++){
        x = i*deltax;
        y = j*deltax;
        ij = i*size + j;
        arg = pow((x - xcent)/xspread,2) +  pow((y - ycent)/yspread,2);
        tseries[ij] = amp * temporal * exp(-arg);       
      }
    }
  }
  else if( mode=="Ramp" ) { // ramp concentration pattern
    if(0 == (fmod((t-ts),stepwidth)-stepwidth) ){
      printf("%f\n", t);
      for(long i=0; i<nodes; i++){
        tseries[i] = tseries[i] + stepheight;
      }
    }
  }
  else if( mode=="GaussPulse" ) { // gaussian pulse
    for(long i=0; i<nodes; i++){
      tseries[i]=amp * (1 / (sqrt(6.2831853)*pdur)) * 
      (exp(-0.5*pow((t-tpeak), 2) / (pdur*pdur)));
    }
  }
}
    //int width = 25; // width of square of nodes to stimulate
      //for(int i=int(sqrt(nodes))/2-width; i<int(sqrt(nodes))/2+width; i++)
        //for(int j=int(sqrt(nodes))/2-width; j<int(sqrt(nodes))/2+width; j++)
          //tseries[int(i+j*sqrt(nodes))]
