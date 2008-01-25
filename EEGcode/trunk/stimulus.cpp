/***************************************************************************
                          stimulus.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "stimulus.h"

Stimulus::Stimulus(){
  idum=-94761025;
  random.ran(idum); // initialize the random number generator
}
Stimulus::~Stimulus(){
}
 
void Stimulus::init(ifstream& inputf){
  inputf.ignore(200,58); //throwaway everything before colon character
  inputf >> mode;
  inputf.ignore(200,58); //throwaway everything before colon character
  inputf >> ts;
  switch (mode) {
    case 1: //  Pulse stimulus pattern 
      inputf.ignore(200,58); // throwaway everything before colon character
      inputf >> amp;
      inputf.ignore(200,58); // throwaway everything before colon character
      inputf >> pdur;
      inputf.ignore(200,58); // throwaway everything before colon character
      inputf >> tperiod;
      break;
    case 2: //  White noise stimulus pattern 
      inputf.ignore(200,58); // throwaway everything before colon character
      inputf >> amp;
      break;
    case 3: //  Sinusoidal stimulus pattern 
      inputf.ignore(200,58); // throwaway everything before colon character
      inputf >> amp;
      inputf.ignore(200,58); // throwaway everything before colon character
      inputf >> freq;
      break;
    case 4: //  Coherent white noise stimulus pattern 
      inputf.ignore(200,58); // throwaway everything before colon character
      inputf >> amp;
      break;
    default: // No stimulation
      break;
    }
}

void Stimulus::dump(ofstream& dumpf){
  dumpf << "Stimulus mode :" << mode << " " << endl;
  dumpf << "Time to start of stimulus :" << ts << " ";
  switch (mode) {
    case 1: //  Pulse stimulus pattern 
      dumpf << "Alpha :" << amp << " ";
      dumpf << "Pulse Duration :" << pdur << " ";
      dumpf << "Pulse repetition period :" << tperiod << endl;
      break;
    case 2: //  White noise stimulus pattern 
      dumpf << "Alpha :" << amp << endl;
      break;
    case 3: //  Sinusoidal stimulus pattern 
      dumpf << "Alpha :" << amp << " ";
      dumpf << "Modulation frequency :" << freq << endl;
      break;
    case 4: //  Coherent white noise stimulus pattern 
      dumpf << "Alpha :" << amp << endl;
      break;
    default: // No stimulation
      break;
    }
}

void Stimulus::restart(ifstream& restartf){
  restartf.ignore(200,58); //throwaway every before colon character
  restartf >> mode;
  restartf.ignore(200,58); //throwaway every before colon character
  restartf >> ts;
  switch (mode) {
    case 1: //  Pulse stimulus pattern 
      restartf.ignore(200,58); // throwaway everything before colon character
      restartf >> amp;
      restartf.ignore(200,58); // throwaway everything before colon character
      restartf >> pdur;
      restartf.ignore(200,58); // throwaway everything before colon character
      restartf >> tperiod;
      break;
    case 2: //  White noise stimulus pattern 
      restartf.ignore(200,58); // throwaway everything before colon character
      restartf >> amp;
      break;
    case 3: //  Sinusoidal stimulus pattern 
      restartf.ignore(200,58); // throwaway everything before colon character
      restartf >> amp;
      restartf.ignore(200,58); // throwaway everything before colon character
      restartf >> freq;
      break;
    case 4: //  Coherent white noise stimulus pattern 
      restartf.ignore(200,58); // throwaway everything before colon character
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
	// For efficiency reasons gaussian random deviates are usually calculated in pairs.
	// This is the reason for slightly more complex updating routine here
	//
        float deviate1, deviate2;
	float *p;
	p=Q;
        for(long i=0; i<nodes-1; i+=2){ // if nodes is even then update every point in Q[]
		                        // if nodes is odd then update all but the last point in Q[]
	  gaussian(deviate1,deviate2);	
          *p++=amp*deviate1;
          *p++=amp*deviate2;
        }
	if(nodes%2){ // if nodes is odd update last point which was otherwise not updated above
	  gaussian(deviate1,deviate2);
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
	gaussian(deviate1,deviate2);
	for(long i=0; i<nodes; i++){
          Q[i]=amp*deviate1;
        }
        break;
      }
      case 5:{ // Spatially white noise temporally a pulse pattern
	      // Similar to Rennie WPULSE
        if(fmod((t-ts),tperiod) < pdur){
	  //
	  // For efficiency reasons gaussian random deviates are usually calculated in pairs.
	  // This is the reason for slightly more complex updating routine here
	  //
            float deviate1, deviate2;
	    float *p;
	    p=Q;
            for(long i=0; i<nodes-1; i+=2){ // if nodes is even then update every point in Q[]
                                            // if nodes is odd then update all but the last point in Q[]
	      gaussian(deviate1,deviate2);	
              *p++=amp*deviate1;
              *p++=amp*deviate2;
            }
            if(nodes%2){ // if nodes is odd update last point which was otherwise not updated above
	      gaussian(deviate1,deviate2);
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

//
// Returns two gaussian random deviates
//  following a algorithm suggested by Knuth
//
void Stimulus::gaussian(float& deviate1, float& deviate2){
  double x, y, norm, factor;
  do {
    x=2*random.ran(idum) - 1; // throw two uniform deviates in square circumscribing unit circle
    y=2*random.ran(idum) - 1;
    norm = x*x+y*y; 
  } while (norm > 1.0 || norm == 0);  // test whether in unit circle
  factor=sqrt(-2.0*log(norm)/norm);
  deviate1=static_cast<float>(x*factor);
  deviate2=static_cast<float>(y*factor);
}

