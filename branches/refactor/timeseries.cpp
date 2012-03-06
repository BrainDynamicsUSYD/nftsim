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

void Timeseries::init( Configf& configf )
{
  random=0; // Set pointer to null in case random is not created
  seed=-98716872;
  configf.Param("Mode",mode);//configf>>mode;
  configf.Param("Onset",t); t = -t;
  if( mode=="Composite" ) {
    // take in stimulus transition times and specifications of each stimulus
    int nStim; configf.Param("Number of Stimuli",nStim);
    for( int i=0; i<nStim; i++ ) {
      // beware of non-unique indexing
      Timeseries* p = new Timeseries(nodes,deltat,index*10+i+1);
      configf>>*p;
      sarray.push_back(p);
    }
  }
  else if( mode=="Const" ) { // constant noise
    configf.Param("Mean",mean);
  }
  else if( mode=="White" ) { // white noise
    configf.Optional("Ranseed",seed);
    configf.Param("Amplitude",amp);
    random = new Random(seed);
    configf.Param("Mean",mean);
  }
  else if( mode=="CoherentWhite" ) { // spatially homogeneous white noise
    configf.Optional("Ranseed",seed);
    configf.Param("Amplitude",amp);
    random = new Random(seed);
    configf.Param("Mean",mean);
  }
  else if( mode=="Pulse" ) { // periodic pulse pattern
    configf.Param("Amplitude",amp);
    configf.Param("Width",pdur);
    if( !configf.Optional("Rep Rate",tperiod) )
      tperiod = 99999999; // if repetition period not specified, do just one pulse
  }
  else if( mode=="Sine" ) { // sinusoidal stimuli
    configf.Param("Amplitude",amp);
    configf.Param("Frequency",freq);
  }
  else if( mode=="Gaussian" ) { // spatial and temporal gaussian
    configf.Param("Amplitude",amp);
    configf.Param("Time to peak of stimulus",tpeak);
    configf.Param("Pulse Duration",pdur);
    configf.Param("Grid Spacing",deltax);
    configf.Param("x location",xcent);
    configf.Param("y location",ycent);
    configf.Param("x spread",xspread);
    configf.Param("y spread",yspread);
  }
  else if( mode=="Ramp" ) { // ramp concentration pattern
    configf.Param("Step height",stepheight);
    configf.Param("Step width",stepwidth);
  }
  else if( mode=="GaussPulse" ) { // gaussian pulse
    configf.Param("Amplitude",amp);
    configf.Param("Pulse Duration",pdur);
    configf.Param("Time at peak",tpeak);
  }
  else if( mode=="MNS" ) { // median nerve stimulation
    configf.Param("N20 width",xspread);
    configf.Param("N20 height",xcent);
    configf.Param("P25 width",yspread);
    configf.Param("P25 height",ycent);
  }
}

void Timeseries::restart( Restartf& restartf )
{
}

void Timeseries::dump( Dumpf& dumpf ) const
{
  dumpf << " Mode:" << mode << " ";
  //dumpf << "Onset: " << onset << " ";
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
    dumpf << " Width: " << pdur;
    dumpf << " Rep Rate: " << tperiod << endl;
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
  else if( mode=="MNS" ) { // median nerve stimulation
    dumpf << "N20 width:" << xspread;
    dumpf << "N20 height:" << xcent;
    dumpf << "P25 width:" << yspread;
    dumpf << "P25 height:" << ycent << endl;
  }
}

Timeseries::Timeseries( int nodes, double deltat, int index )
    : NF(nodes,deltat,index)
{
}

Timeseries::~Timeseries(){
  if(random) delete random;
  for( unsigned int i=0; i<sarray.size(); i++ )
    if(sarray[i]) delete sarray[i];
}

void Timeseries::step(void)
{
  t += deltat;
}

void Timeseries::fire( vector<double>& Q ) const
{
if( t<0 ) return;
  if( mode == "Composite" ) {
    for( unsigned int i=0; i<sarray.size(); i++ ) {
      sarray[i]->step();
      sarray[i]->fire(Q);
    }
  }
  else if( mode=="Const" ) { // constant noise
    for( int i=0; i<nodes; i++ )
      Q[i] += mean;
  }
  else if( mode=="White" ) { // white noise
    // For efficiency reasons random.gaussian random deviates are usually
    // calculated in pairs. This is the reason for slightly more complex
    // updating routine here
    double deviate1, deviate2;
    // if nodes is even then update every point in tseries[]
    // if nodes is odd then update all but the last point in tseries[]
    for( int i=0; i<nodes-1; i+=2 ){
      random->gaussian(deviate1,deviate2);	
      Q[i]   += amp*deviate1 + mean;
      Q[i+1] += amp*deviate2 + mean;
    }
    // if nodes is odd update last point which was otherwise not updated above
    if(nodes%2){
      random->gaussian(deviate1,deviate2);
      Q[nodes] += amp*deviate1 + mean;
    }
  }
  else if( mode=="CoherentWhite" ) { // spatially homogeneous white noise
    double deviate1, deviate2;
    random->gaussian(deviate1,deviate2);
    for( int i=0; i<nodes; i++ )
      Q[i]=amp*deviate1 + mean;
  }
  else if( mode=="Pulse" ) { // periodic pulse pattern
    if( fmod(t,tperiod)<pdur )
      for(int i=0; i<nodes; i++)
        Q[i] += amp;
  }
  else if( mode=="Sine" ) { // sinusoidal stimuli
    for(int i=0; i<nodes; i++)
      Q[i]=amp*sin(6.2831853F*freq*t);
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
    for(int i=0; i<size; i++){
      for(int j=0; j<size; j++){
        x = i*deltax;
        y = j*deltax;
        ij = i*size + j;
        arg = pow((x - xcent)/xspread,2) +  pow((y - ycent)/yspread,2);
        Q[ij] = amp * temporal * exp(-arg);       
      }
    }
  }
  else if( mode=="Ramp" ) { // ramp concentration pattern
    if(0 == (fmod(t,stepwidth)-stepwidth) ){
      printf("%f\n", t);
      for(int i=0; i<nodes; i++){
        Q[i] = Q[i] + stepheight;
      }
    }
  }
  else if( mode=="GaussPulse" ) { // gaussian pulse
    for(int i=0; i<nodes; i++){
      Q[i]=amp * (1 / (sqrt(6.2831853)*pdur)) * 
      (exp(-0.5*pow((t-tpeak), 2) / (pdur*pdur)));
    }
  }
  else if( mode=="MNS" ) { // median nerve stimulation
    if( t<xspread )
      for( int i=0; i<nodes; i++ )
        Q[i] = -xcent*sin(3.141592654*t/xspread);
    else if( t<xspread+yspread )
      for( int i=0; i<nodes; i++ )
        Q[i] = ycent*sin(3.141592654*(t-xspread)/yspread);
	;
  }
}
    //int width = 25; // width of square of nodes to stimulate
      //for(int i=int(sqrt(nodes))/2-width; i<int(sqrt(nodes))/2+width; i++)
        //for(int j=int(sqrt(nodes))/2-width; j<int(sqrt(nodes))/2+width; j++)
          //tseries[int(i+j*sqrt(nodes))]
