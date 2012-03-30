#include <cmath>
#include<iostream>
using std::endl;
#include "timeseries.h"

void Timeseries::init( Configf& configf )
{
  random=0; // Set pointer to null in case random is not created
  seed=-98716872;
  configf.param("Mode",mode);//configf>>mode;
  configf.param("Onset",t); t = -t;
  if( mode=="Composite" ) {
    // take in stimulus transition times and specifications of each stimulus
    int nStim; configf.param("Number of Stimuli",nStim);
    for( int i=0; i<nStim; i++ ) {
      // beware of non-unique indexing
      Timeseries* p = new Timeseries(nodes,deltat,index*10+i+1);
      configf>>*p;
      sarray.push_back(p);
    }
  }
  else if( mode=="Const" ) { // constant noise
    configf.param("Mean",mean);
  }
  else if( mode=="White" ) { // white noise
    configf.optional("Ranseed",seed);
    configf.param("Amplitude",amp);
    random = new Random(seed);
    configf.param("Mean",mean);
  }
  else if( mode=="CoherentWhite" ) { // spatially homogeneous white noise
    configf.optional("Ranseed",seed);
    configf.param("Amplitude",amp);
    random = new Random(seed);
    configf.param("Mean",mean);
  }
  else if( mode=="Pulse" ) { // periodic pulse pattern
    configf.param("Amplitude",amp);
    configf.param("Width",pdur);
    if( !configf.optional("Rep Rate",tperiod) )
      tperiod = 99999999; // if repetition period not specified, do just one pulse
  }
  else if( mode=="Sine" ) { // sinusoidal stimuli
    configf.param("Amplitude",amp);
    configf.param("Frequency",freq);
  }
  else if( mode=="Gaussian" ) { // spatial and temporal gaussian
    configf.param("Amplitude",amp);
    configf.param("Time to peak of stimulus",tpeak);
    configf.param("Pulse Duration",pdur);
    configf.param("Grid Spacing",deltax);
    configf.param("x location",xcent);
    configf.param("y location",ycent);
    configf.param("x spread",xspread);
    configf.param("y spread",yspread);
  }
  else if( mode=="Ramp" ) { // ramp concentration pattern
    configf.param("Step height",stepheight);
    configf.param("Step width",stepwidth);
  }
  else if( mode=="GaussPulse" ) { // gaussian pulse
    configf.param("Amplitude",amp);
    configf.param("Pulse Duration",pdur);
    configf.param("Time at peak",tpeak);
  }
  else if( mode=="PAS" ) { // paired associative stimulation
    configf.param("N20 width",xspread);
    configf.param("N20 height",xcent);
    configf.param("P25 width",yspread);
    configf.param("P25 height",ycent);
    configf.param("TMS",amp);
    configf.param("ISI",tpeak);
    configf.param("Radius",deltax);
  }
}

void Timeseries::restart( Restartf& restartf )
{
}

void Timeseries::dump( Dumpf& dumpf ) const
{
  /*dumpf << " Mode:" << mode << " ";
  //dumpf << "Onset: " << onset << " ";
  if( mode=="Composite" ) {
    dumpf << "Stimuli: " << int(sarray.size()) << endl;
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
  }*/
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
  if( mode == "Composite" ) {
    if( t<0 ) return;
    for( unsigned int i=0; i<sarray.size(); i++ ) {
      sarray[i]->step();
      sarray[i]->fire(Q);
    }
  }
  else if( mode=="Const" ) { // constant noise
    if( t<0 ) return;
    for( int i=0; i<nodes; i++ )
      Q[i] += mean;
  }
  else if( mode=="White" ) { // white noise
    if( t<0 ) return;
    // For efficiency reasons random.gaussian random deviates are usually
    // calculated in pairs. This is the reason for slightly more complex
    // updating routine here
    double deviate1, deviate2;
    // if nodes is even then update every point in tseries[]
    // if nodes is odd then update all but the last point in tseries[]
    for( int i=0; i<nodes-1; i+=2 ) {
      random->gaussian(deviate1,deviate2);	
      Q[i]   += amp*deviate1 + mean;
      Q[i+1] += amp*deviate2 + mean;
    }
    // if nodes is odd update last point which was otherwise not updated above
    if(nodes%2) {
      random->gaussian(deviate1,deviate2);
      Q[nodes-1] += amp*deviate1 + mean;
    }
  }
  else if( mode=="CoherentWhite" ) { // spatially homogeneous white noise
    if( t<0 ) return;
    double deviate1, deviate2;
    random->gaussian(deviate1,deviate2);
    for( int i=0; i<nodes; i++ )
      Q[i] += amp*deviate1 + mean;
  }
  else if( mode=="Pulse" ) { // periodic pulse pattern
    if( t<0 ) return;
    if( fmod(t,tperiod)<pdur )
      //for( int i=0; i<nodes; i++ )
      for( int i=1274; i<1275; i++ )
        Q[i] += amp;
  }
  else if( mode=="Sine" ) { // sinusoidal stimuli
    if( t<0 ) return;
    for( int i=0; i<nodes; i++ )
      Q[i] += amp*sin(6.2831853F*freq*t);
  }
  else if( mode=="Gaussian" ) { // spatial and temporal gaussian
    if( t<0 ) return;
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
        Q[ij] += amp * temporal * exp(-arg);       
      }
    }
  }
  else if( mode=="Ramp" ) { // ramp concentration pattern
    if( t<0 ) return;
    if( 0 == (fmod(t,stepwidth)-stepwidth ) ){
      printf("%f\n", t);
      for( int i=0; i<nodes; i++ ) {
        Q[i] += Q[i] + stepheight;
      }
    }
  }
  else if( mode=="GaussPulse" ) { // gaussian pulse
    if( t<0 ) return;
    for( int i=0; i<nodes; i++ ) {
      Q[i] += amp * (1 / (sqrt(6.2831853)*pdur)) *
          (exp(-0.5*pow((t-tpeak), 2) / (pdur*pdur)));
    }
  }
  else if( mode=="PAS" ) { // median nerve stimulation
    int width = sqrt(nodes);
    if( width%2 == 0 ) { // even
      if( 0<t && t<=xspread )
        for(int i=width/2-deltax; i<width/2+deltax; i++)
          for(int j=width/2-deltax; j<width/2+deltax; j++)
            Q[i+j*width] += -xcent*sin(3.141592654*t/xspread);
      else if( xspread<t && t<xspread+yspread )
        for(int i=width/2-deltax; i<width/2+deltax; i++)
          for(int j=width/2-deltax; j<width/2+deltax; j++)
            Q[i+j*width] += ycent*sin(3.141592654*(t-xspread)/yspread);
      if( xspread/2+tpeak<t && t<=xspread/2+tpeak+4e-3 )
        for(int i=width/2-deltax+1; i<width/2+deltax; i++)
          for(int j=width/2-deltax+1; j<width/2+deltax; j++)
            Q[i+j*width] += amp;
    } else { // odd
      if( 0<t && t<=xspread )
        for(int i=width/2-deltax+1; i<width/2+deltax; i++)
          for(int j=width/2-deltax+1; j<width/2+deltax; j++)
            Q[i+j*width] += -xcent*sin(3.141592654*t/xspread);
      else if( xspread<t && t<xspread+yspread )
        for(int i=width/2-deltax+1; i<width/2+deltax; i++)
          for(int j=width/2-deltax+1; j<width/2+deltax; j++)
            Q[i+j*width] += ycent*sin(3.141592654*(t-xspread)/yspread);
      if( xspread/2+tpeak<t && t<=xspread/2+tpeak+4e-3 )
        for(int i=width/2-deltax+1; i<width/2+deltax; i++)
          for(int j=width/2-deltax+1; j<width/2+deltax; j++)
            Q[i+j*width] += amp;
    }
    /*if( t<xspread )
      for( int i=0; i<nodes; i++ )
        Q[i] += -xcent*sin(3.141592654*t/xspread);
    else if( t<xspread+yspread )
      for( int i=0; i<nodes; i++ )
        Q[i] += ycent*sin(3.141592654*(t-xspread)/yspread);*/
  }
}
