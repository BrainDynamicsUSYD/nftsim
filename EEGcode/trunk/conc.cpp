/***************************************************************************
                          conc.cpp  -  
                             -------------------
    copyright            : (C) 2007 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "conc.h"
#include <math.h>

Concentration::Concentration():t(0){ //start time is t=0
}
Concentration::~Concentration(){
}
 
float Concentration::init(Istrm& inputf){
  inputf.validate("Concentration mode",58);
  inputf >> mode;
  inputf.validate("Time to start of Concentration",58);
  inputf >> ts;
  switch (mode) {
    case 1: //  Pulse Concentration pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      inputf.validate("Pulse Duration",58);
      inputf >> pdur;
      inputf.validate("Pulse repetition period",58);
      inputf >> tperiod;
      mean=0.0;
      break;
    case 2: //  White noise Concentration pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      inputf.validate("Mean",58);
      inputf >> mean;
      break;
    case 3: //  Sinusoidal Concentration pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      inputf.validate("Modulation Frequency",58);
      inputf >> freq;
      break;
    case 4: //  Coherent white noise Concentration pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
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
    default: // No concentration pattern
      break;
    }
  return mean;
}

void Concentration::dump(ofstream& dumpf){
  dumpf << "Concentration mode:" << mode << " ";
  dumpf << "Time to start of Concentration:" << ts << " ";
  switch (mode) {
    case 1: //  Pulse Concentration pattern 
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Pulse Duration:" << pdur << " ";
      dumpf << "Pulse repetition period:" << tperiod << " ";
      break;
    case 2: //  White noise Concentration pattern 
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Mean:" << mean << " ";
      break;
    case 3: //  Sinusoidal Concentration pattern 
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Modulation frequency :" << freq << " ";
      break;
    case 4: //  Coherent white noise Concentration pattern 
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
    default: // No concentration
      break;
    }
}

float Concentration::restart(Istrm& restartf){
  restartf.validate("Concentration mode",58);
  restartf >> mode;
  restartf.validate("Time to start of Concentration",58);
  restartf >> ts;
  switch (mode) {
    case 1: //  Pulse Concentration pattern 
      restartf.validate("Amplitude",58);
      restartf >> amp;
      restartf.validate("Pulse Duration",58);
      restartf >> pdur;
      restartf.validate("Pulse repetition period",58);
      restartf >> tperiod;
      mean=0.0;
      break;
    case 2: //  White noise Concentration pattern 
      restartf.validate("Amplitude",58);
      restartf >> amp;
      restartf.validate("Mean",58);
      restartf >> mean;
      break;
    case 3: //  Sinusoidal Concentration pattern 
      restartf.validate("Amplitude",58);
      restartf >> amp;
      restartf.validate("Modulation Frequency",58);
      restartf >> freq;
      break;
    case 4: //  Coherent white noise Concentration pattern 
      restartf.validate("Amplitude",58);
      restartf >> amp;
      restartf.validate("Mean",58);
      restartf >> mean;
      break;
    case 6: //  Spatial-Temporal Gaussian    
      restartf.validate("Amplitude",58);
      restartf >> amp;
      restartf.validate("Time to peak of stimulus",58);
      restartf >> tpeak;
      restartf.validate("Pulse Duration",58);
      restartf >> pdur;
      restartf.validate("Grid Spacing",58);
      restartf >> deltax;
      restartf.validate("x location",58);
      restartf >> xcent;
      restartf.validate("y location",58);
      restartf >> ycent;
      restartf.validate("x spread",58);
      restartf >> xspread;
      restartf.validate("y spread",58);
      restartf >> yspread;
      break;
    default: // No concentration
      break;
    }
  return mean;
}

void Concentration::get(double timestep, double *Q, const long nodes){
  if(t>=ts){
    switch (mode) {
      case 1:{ // Pulse Concentration pattern 
        if(fmod((t-ts),tperiod) < pdur){
	  for(long i=0; i<nodes; i++){
            Q[i]=amp;
          }
	} else {
          for(long i=0; i<nodes; i++)
            Q[i]=0.0F;
	}
        break;
      }
      case 2:{ // White noise pulse Concentration pattern
	//
	// For efficiency reasons random.gaussian random deviates are usually calculated in pairs.
	// This is the reason for slightly more complex updating routine here
	//
        double deviate1, deviate2;
	double *p;
	p=Q;
        for(long i=0; i<nodes-1; i+=2){ // if nodes is even then update every point in Q[]
		                        // if nodes is odd then update all but the last point in Q[]
	  random.gaussian(deviate1,deviate2);	
          *p++=amp*deviate1 + mean;
          *p++=amp*deviate2 + mean;
        }
	if(nodes%2){ // if nodes is odd update last point which was otherwise not updated above
	  random.gaussian(deviate1,deviate2);
	  *p=amp*deviate1 + mean;
	}
        break;
      }
      case 3:{ // Sinusoidal Concentration pattern
	for(long i=0; i<nodes; i++){
	  Q[i]=amp*sin(6.2831853F*freq*t);
	}
	break;
      }
      case 4:{ // Coherent white noise Concentration pattern
        double deviate1, deviate2;
	random.gaussian(deviate1,deviate2);
	for(long i=0; i<nodes; i++){
          Q[i]=amp*deviate1 + mean;
        }
        break;
      }
      case 5:{ // Spatially white noise temporally a pulse pattern
       if(fmod((t-ts),tperiod) < pdur){
	  //
	  // For efficiency reasons random.gaussian random deviates are usually calculated in pairs.
	  // This is the reason for slightly more complex updating routine here
	  //
            double deviate1, deviate2;
	    double *p;
	    p=Q;
            for(long i=0; i<nodes-1; i+=2){ // if nodes is even then update every point in Q[]
                                            // if nodes is odd then update all but the last point in Q[]
	      random.gaussian(deviate1,deviate2);	
              *p++=amp*deviate1;
              *p++=amp*deviate2;
            }
            if(nodes%2){ // if nodes is odd update last point which was otherwise not updated above
	      random.gaussian(deviate1,deviate2);
              *p=amp*deviate1;
            }
        } else {
          for(long i=0; i<nodes; i++)
            Q[i]=0.0F;
	}
        break;
      }
      case 6: { // Spatially Gaussian pulse, temporally Gaussian pulse
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
            Q[ij] = amp * temporal * exp(-arg);       
          }
        }
        break;
      }

      default:{ // Default is No concentration pattern
        for(long i=0; i<nodes; i++)
          Q[i]=0.0F;
        break;
      }
    }
  }
  else {
    for(long i=0; i<nodes; i++) // Zero conc. in (<ts) period
          Q[i]=0.0F;
  }
  t+=timestep; // Update the current time
}
