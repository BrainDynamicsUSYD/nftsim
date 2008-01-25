/***************************************************************************
                          stimulus.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "stimulus.h"
#include <math.h>

Stimulus::Stimulus():t(0){ //start time is t=0
}
Stimulus::~Stimulus(){
}
 
void Stimulus::init(Istrm& inputf){
  inputf.validate("Stimulus mode",58);
  inputf >> mode;
  inputf.validate("Time to start of stimulus",58);
  inputf >> ts;
  switch (mode) {
    case 1: //  Pulse stimulus pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      inputf.validate("Pulse Duration",58);
      inputf >> pdur;
      inputf.validate("Pulse repetition period",58);
      inputf >> tperiod;
      break;
    case 2: //  White noise stimulus pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      break;
    case 3: //  Sinusoidal stimulus pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      inputf.validate("Modulation Frequency",58);
      inputf >> freq;
      break;
    case 4: //  Coherent white noise stimulus pattern 
      inputf.validate("Amplitude",58);
      inputf >> amp;
      break;
    default: // No stimulation
      break;
    }
}

void Stimulus::dump(ofstream& dumpf){
  dumpf << "Stimulus mode:" << mode << " " << endl;
  dumpf << "Time to start of stimulus:" << ts << " ";
  switch (mode) {
    case 1: //  Pulse stimulus pattern 
      dumpf << "Amplitude:" << amp << " ";
      dumpf << "Pulse Duration:" << pdur << " ";
      dumpf << "Pulse repetition period:" << tperiod << endl;
      break;
    case 2: //  White noise stimulus pattern 
      dumpf << "Amplitude:" << amp << endl;
      break;
    case 3: //  Sinusoidal stimulus pattern 
      dumpf << "Amplitude:" << amp << endl;
      dumpf << "Modulation frequency :" << freq << endl;
      break;
    case 4: //  Coherent white noise stimulus pattern 
      dumpf << "Amplitude:" << amp << endl;
      break;
    default: // No stimulation
      break;
    }
}

void Stimulus::restart(Istrm& restartf){
  restartf.validate("Stimulus mode",58);
  restartf >> mode;
  restartf.validate("Time to start of stimulus",58);
  restartf >> ts;
  switch (mode) {
    case 1: //  Pulse stimulus pattern 
      restartf.validate("Amplitude",58);
      restartf >> amp;
      restartf.validate("Pulse Duration",58);
      restartf >> pdur;
      restartf.validate("Pulse repetition period",58);
      restartf >> tperiod;
      break;
    case 2: //  White noise stimulus pattern 
      restartf.validate("Amplitude",58);
      restartf >> amp;
      break;
    case 3: //  Sinusoidal stimulus pattern 
      restartf.validate("Amplitude",58);
      restartf >> amp;
      restartf.validate("Modulation Frequency",58);
      restartf >> freq;
      break;
    case 4: //  Coherent white noise stimulus pattern 
      restartf.validate("Amplitude",58);
      restartf >> amp;
      break;
    default: // No stimulation
      break;
    }
}

void Stimulus::getQstim(float timestep, float *Q, const long nodes){
  if(t>ts){
    switch (mode) {
      case 1:{ // Pulse stimulus pattern 
	      // Similar to Rennie CPULSE
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
      case 2:{ // White noise pulse stimulus pattern
	      // Similar to Rennie WWGAUSS
	//
	// For efficiency reasons random.gaussian random deviates are usually calculated in pairs.
	// This is the reason for slightly more complex updating routine here
	//
        float deviate1, deviate2;
	float *p;
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
        break;
      }
      case 3:{ // Sinusoidal stimulus pattern
	      // Similar to Rennie SINE
	for(long i=0; i<nodes; i++){
	  Q[i]=amp*sin(6.2831853F*freq*t);
	}
	break;
      }
      case 4:{ // Coherent white noise stimulus pattern
	      // Similar to Rennie CWGAUSS
        float deviate1, deviate2;
	random.gaussian(deviate1,deviate2);
	for(long i=0; i<nodes; i++){
          Q[i]=amp*deviate1;
        }
        break;
      }
      case 5:{ // Spatially white noise temporally a pulse pattern
	      // Similar to Rennie WPULSE
        if(fmod((t-ts),tperiod) < pdur){
	  //
	  // For efficiency reasons random.gaussian random deviates are usually calculated in pairs.
	  // This is the reason for slightly more complex updating routine here
	  //
            float deviate1, deviate2;
	    float *p;
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
      default:{ // Default is No stimulation
        for(long i=0; i<nodes; i++)
          Q[i]=0.0F;
        break;
      }
    }
  }
  else {
    for(long i=0; i<nodes; i++) // No stimulation in the prestimulus (<ts) period
          Q[i]=0.0F;
  }
  t+=timestep; // Update the current time
}
