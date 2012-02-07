/***************************************************************************
                          timeseries.cpp  -  a generic time series generator 
                             -------------------
    copyright            : (C) 2008 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "timeseries.h"
#include <math.h>
#include<sstream>
#include<cstdlib>
#include<iostream>
using std::stringstream;
using std::endl;

Timeseries::Timeseries(const char * typeid1, const char * typeid2,Istrm& inputf):id1(typeid1), id2(typeid2){
  random=0; // Set pointer to null in case random is not created
  seed=-98716872;
  int optionnum;
  stringstream message1(stringstream::in | stringstream::out);
  message1 << id1 << " mode";
  inputf.validate(message1.str().c_str(),58);
  inputf >> mode;
  stringstream message2(stringstream::in | stringstream::out);
  message2 << "Time to start" << id2;
  inputf.validate(message2.str().c_str(),58);
  inputf >> ts;
  std::string input;
  switch (mode) {
    case 0: //  Composite stimulus
      // take in stimulus transition times and specifications of each stimulus
      message1.str(""); message1<<ts;
      ts = 0;
      for( std::string input = message1.str(); input!= "$"; inputf>>input )
        stimtime.push_back( atof(input.c_str()) );
      stimulus.resize( stimtime.size(), 0 );
      for( unsigned int i=0; i<stimulus.size(); i++ )
        stimulus[i] = new Timeseries( "Stimulus"," of stimulus",inputf );
      break;
    case 1: //  Pulse pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      inputf.validate("Pulse Duration",58);
      inputf >> pdur;
      inputf.validate("Pulse repetition period",58);
      inputf >> tperiod;
      inputf.validate("Background",58);
      inputf >> mean;
      break;
    case 2: //  White noise pattern
      optionnum=inputf.choose("Amplitude:1 Ranseed:2",58);
      if(1==optionnum){
        inputf >> amp;
      } else {
        inputf >> seed;
        inputf.validate("Amplitude",58);
        inputf >> amp;
      }
      random = new Random(seed);
      inputf.validate("Mean",58);
      inputf >> mean;
      break;
    case 3: //  Sinusoidal pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      inputf.validate("Modulation Frequency",58);
      inputf >> freq;
      break;
    case 4: //  Coherent white noise pattern 
      optionnum=inputf.choose("Amplitude:1 Ranseed:2",58);
      if(1==optionnum){
        inputf >> amp;
      } else {
        inputf >> seed;
        inputf.validate("Amplitude",58);
        inputf >> amp;
      }
      random = new Random(seed);
      inputf.validate("Mean",58);
      inputf >> mean;
      break;
    case 6: //  Spatial-Temporal Gaussian    
      inputf.validate("Amplitude",58);
      inputf >> amp;
      inputf.validate("Time to peak of stimulus",58);
      inputf >> tpeak;
      inputf.validate("Pulse Duration",58);
      inputf >> pdur;
      inputf.validate("Grid Spacing",58);
      inputf >> deltax;
      inputf.validate("x location",58);
      inputf >> xcent;
      inputf.validate("y location",58);
      inputf >> ycent;
      inputf.validate("x spread",58);
      inputf >> xspread;
      inputf.validate("y spread",58);
      inputf >> yspread;
      break;
    case 7: // Constant
      inputf.validate("Mean",58);
      inputf >> mean;
      break;
    case 8: //  JC's Ramped input
      inputf.validate("Step height",58);
      inputf >> stepheight;
      inputf.validate("Step width",58);
      inputf >> stepwidth;
      break;
    case 9: //  JC's Gaussian Pulse pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      inputf.validate("Pulse Duration",58);
      inputf >> pdur;
      inputf.validate("Time at peak",58);
      inputf >> tpeak;
      break;
    case 10: // paired associative stimulation (PAS)
      // constant or Gaussian background
      random = new Random(seed);
      inputf.validate("Background",58);
      inputf >> amp;
      inputf >> mean;
      // median nerve stimulus (MNS), with the N20 and P25 peaks
      inputf.validate("N20",58);
      inputf >> xcent; // amplitude of N20
      inputf >> xspread; // half period of N20
      inputf.validate("P25",58);
	  inputf >> ycent; // amplitude of P25
      inputf >> yspread; // half period of P25
	  // transcranial magnetic stimulation (TMS)
      inputf.validate("TMS",58);
      inputf >> stepheight; // amplitude of TMS pulse
      inputf >> stepwidth; // duration of TMS pulse
      inputf.validate("ISI",58);
      inputf >> tpeak; // time interval between N20 and TMS, +ve=>MNS before TMS
	  break;
    default: // No pattern
      break;
    }
}

Timeseries::~Timeseries(){
  if(random) delete random;
  for( unsigned int i=0; i<stimulus.size(); i++ )
    if(stimulus[i]) delete stimulus[i];
}

void Timeseries::dump(std::ofstream& dumpf){
  dumpf << id1 << " mode:" << mode << " ";
  dumpf << "Time to start" << id2 << ":" << ts << " ";
  switch (mode) {
    case 0: // Composite pattern
      for( unsigned int i=0; i<stimtime.size(); i++ )
        dumpf<<" "<<stimtime[i];
      dumpf<<" $"<<endl;
      for( unsigned int i=0; i<stimulus.size(); i++ )
        stimulus[i]->dump(dumpf);
      break;
    case 1: //  Pulse pattern 
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Pulse Duration:" << pdur << " ";
      dumpf << "Pulse repetition period:" << tperiod << " ";
      break;
    case 2: //  White noise pattern 
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Mean:" << mean << " ";
      break;
    case 3: //  Sinusoidal pattern 
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Modulation frequency :" << freq << " ";
      break;
    case 4: //  Coherent white noise pattern 
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Mean:" << mean << " ";
      break;
    case 6: // Spatial-Temporal Gaussian
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Time to peak of stimulus:" << tpeak << " ";
      dumpf << "Pulse Duration:" << pdur << " ";
      dumpf << "Grid SPacing:" << deltax << " ";
      dumpf << "x location:" << xcent << " ";
      dumpf << "y location:" << ycent << " ";
      dumpf << "x spread:" << xspread << " ";
      dumpf << "y spread:" << yspread << " ";
      break;
    case 7: // Constant
      dumpf << "Mean:" << mean << " ";
      break;
    case 8: // Ramped Input
      dumpf << "Step height:" << stepheight << " ";
      dumpf << "Step width:" << stepwidth << " ";
      break;
    case 9: //  Gaussian Pulse pattern
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Pulse Duration:" << pdur << " ";
      dumpf << "Time at peak:" << tpeak << " ";
      break;
    case 10: // paired associative stimulation (PAS)
      dumpf <<"Mean: "<<mean<<" N20: "<<xcent<<" "<<xspread;
      dumpf <<" P25: "<<ycent<<" "<<yspread;
      dumpf <<" TMS: "<<amp<<" "<<pdur;
      dumpf <<" ISI: "<<tpeak<<" ";
      break;
    default: // No pattern
      break;
    }
}

void Timeseries::get(double t, double *tseries, const long nodes){
  switch (mode) {
    case 0: // Composite pattern
      if( t<ts ) goto mean;
      for( int i=stimtime.size()-1; i>=0; i-- )
        if( t>stimtime[i] ) {
            stimulus[i]->get( t-stimtime[i], tseries, nodes );
            break;
        }
      break;
    case 1:{ // Pulse pattern 
      for(long i=0; i<nodes; i++)
        tseries[i]=mean;
      if( fmod((t-ts),tperiod)<pdur && t>=ts )
        for(long i=0; i<nodes; i++)
          tseries[i]=amp;
      break;
    }
    case 2:{ // White noise pattern
      if( t<ts ) goto mean;
  //
  // For efficiency reasons random.gaussian random deviates are usually calculated in pairs.
  // This is the reason for slightly more complex updating routine here
  //
      double deviate1, deviate2;
  double *p;
  p=tseries;
      for(long i=0; i<nodes-1; i+=2){ // if nodes is even then update every point in tseries[]
  	              // if nodes is odd then update all but the last point in tseries[]
    random->gaussian(deviate1,deviate2);	
        *p++=amp*deviate1 + mean;
        *p++=amp*deviate2 + mean;
      }
  if(nodes%2){ // if nodes is odd update last point which was otherwise not updated above
    random->gaussian(deviate1,deviate2);
    *p=amp*deviate1 + mean;
  }
      break;
    }
    case 3:{ // Sinusoidal pattern - spatial coherent
      if( t<ts ) goto mean;
  for(long i=0; i<nodes; i++){
    tseries[i]=amp*sin(6.2831853F*freq*t);
  }
  break;
    }
    case 4:{ // Coherent white noise pattern
      if( t<ts ) goto mean;
      double deviate1, deviate2;
        random->gaussian(deviate1,deviate2);
        for(long i=0; i<nodes; i++){
        tseries[i]=amp*deviate1 + mean;
      }
      break;
    }
    case 5:{ // Spatially white noise temporally a pulse pattern
      if( t<ts ) goto mean;
        if(fmod((t-ts),tperiod) < pdur){
        //
        // For efficiency reasons random.gaussian random deviates are usually calculated in pairs.
        // This is the reason for slightly more complex updating routine here
        //
          double deviate1, deviate2;
          double *p;
          p=tseries;
          for(long i=0; i<nodes-1; i+=2){ // if nodes is even then update every point in tseries[]
          // if nodes is odd then update all but the last point in tseries[]
          random->gaussian(deviate1,deviate2);	
          *p++=amp*deviate1;
          *p++=amp*deviate2;
        }
        if(nodes%2){ // if nodes is odd update last point which was otherwise not updated above
          random->gaussian(deviate1,deviate2);
          *p=amp*deviate1;
        }
      } else {
        for(long i=0; i<nodes; i++)
          tseries[i]=0.0F;
      }
      break;
    }
    case 6: { // Spatially Gaussian pulse, temporally Gaussian pulse
      if( t<ts ) goto mean;
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
      break;
    }
    case 7: { // Constant
      goto mean;
    }
    case 8:{ // JC's Ramp Concentration pattern
      if( t<ts ) goto mean;
      if(0 == (fmod((t-ts),stepwidth)-stepwidth) ){
        printf("%f\n", t);
        for(long i=0; i<nodes; i++){
          tseries[i] = tseries[i] + stepheight;
        }
      }
      break;
    }
    case 9: { // JC's Gaussian pulse pattern
      if( t<ts ) goto mean;
      for(long i=0; i<nodes; i++){
        tseries[i]=amp * (1 / (sqrt(6.2831853)*pdur)) * 
        (exp(-0.5*pow((t-tpeak), 2) / (pdur*pdur))); //3.1415926F 
      }
      break;
    }
    case 10:{ // paired associative stimulation (PAS)
      // background stimulation, copied from case 2
      double deviate1, deviate2; double *p=tseries;
      for(long i=0; i<nodes-1; i+=2){
        random->gaussian(deviate1,deviate2);	
        *p++=amp*deviate1 + mean;
        *p++=amp*deviate2 + mean;
      }
      if(nodes%2){
        random->gaussian(deviate1,deviate2);
        *p=amp*deviate1 + mean;
      }
      int width = 25; // width of square of nodes to stimulate
      // median nerve stimulation N20
      if( t>ts && t-ts<xspread ) 
        for(int i=int(sqrt(nodes))/2-width; i<int(sqrt(nodes))/2+width; i++)
          for(int j=int(sqrt(nodes))/2-width; j<int(sqrt(nodes))/2+width; j++)
            tseries[int(i+j*sqrt(nodes))] -= xcent*sin( (t-ts)*3.141592654/xspread );
      // median nerve stimulation P25
      else if( t-ts>=xspread && t-ts<xspread+yspread ) 
        for(int i=int(sqrt(nodes))/2-width; i<int(sqrt(nodes))/2+width; i++)
          for(int j=int(sqrt(nodes))/2-width; j<int(sqrt(nodes))/2+width; j++)
            tseries[int(i+j*sqrt(nodes))] += ycent*sin( (t-ts-xspread)*3.141592654/yspread );
      // transcranial magnetic stimulation
      if( t-ts>=xspread/2+tpeak && t-ts<xspread/2+tpeak+stepwidth){
        for(int i=int(sqrt(nodes))/2-width; i<int(sqrt(nodes))/2+width; i++)
          for(int j=int(sqrt(nodes))/2-width; j<int(sqrt(nodes))/2+width; j++)
            tseries[int(i+j*sqrt(nodes))] += stepheight;
      }
      break;
    }
    mean: // Default is No pattern
    default:{
      for(long i=0; i<nodes; i++)
        tseries[i]=mean;
      break;
    }
  }
}
