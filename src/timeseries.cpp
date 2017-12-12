/** @file timeseries.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "timeseries.h" // Timeseries; TIMESERIES::;

// Other neurofield headers
#include "configf.h"    // Configf;
#include "random.h"     // Random;

// C++ standard library headers
#include <cmath>    // std::pow; std::sqrt; std::sin; std::fmod; M_PI;
#include <iostream> // std::cerr; std::endl;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::fmod;
using std::pow;
using std::sin;
using std::sqrt;
using std::string;
using std::vector;

void Timeseries::init( Configf& configf ) {
  series_size_type superimpose = 1;
  configf.optional("Superimpose", superimpose);
  double duration = 0.0; // Duration of the stimulus.
  double onset = 0.0; // Onset time for the stimulus.
  for( series_size_type i=0; i<superimpose; i++ ) {
    if( superimpose > 0 ) {
      configf.next("Stimulus");
    }
    vector<string> mode = configf.arb("-");

    // Set stimulus onset time.
    if(!configf.optional("Onset", onset)){
      onset = 0.0;
    }
    t = -onset; //Initialise stimulus time relative to onset.

    // Set stimulus duration.
    if(!configf.optional("Duration", duration) ) {
      duration = inf; //Unspecified => whole simulation.
    }

    vector<double> temp_node;
    if( configf.next("Node") ) {
      temp_node = configf.numbers();
    }
    if( temp_node.empty() ) {
      for( size_type j=1; j<=nodes; j++ ) {
        temp_node.push_back(j);
      }
    }
    for(double j : temp_node) {
      if( j > nodes ) {
        cerr<<"Trying to plot node number "<<j
            <<", which is bigger than the highest node index."<<endl;
        exit(EXIT_FAILURE);
      }
    }
    //else
    //node.push_back( temp_node[j]-1 );

    // PUT YOUR TIMEFUNCTION HERE
    if( mode[0]=="Const" ) {
      series.push_back( new TIMESERIES::Const(nodes,deltat,index) );
    } else if( mode[0]=="Pulse" ) {
      series.push_back( new TIMESERIES::Pulse(nodes,deltat,index) );
    } else if( mode[0]=="White" ) {
      series.push_back( new TIMESERIES::White(nodes,deltat,index) );
    } else if( mode[0]=="WhiteCoherent" ) {
      series.push_back( new TIMESERIES::WhiteCoherent(nodes,deltat,index) );
    } else if( mode[0]=="PAS" ) {
      series.push_back( new TIMESERIES::PAS(nodes,deltat,index) );
    } else if( mode[0]=="Burst" ) {
      series.push_back( new TIMESERIES::Burst(nodes,deltat,index) );
    } else if( mode[0]=="Sine" ) {
      series.push_back( new TIMESERIES::Sine(nodes,deltat,index) );
    } else {
      cerr<<"Stimulus mode "<<mode[0].c_str()<<" not found"<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR TIMEFUNCTION HERE
    series[i]->t = t;
    series[i]->duration = duration;
    for(double j : temp_node) {
      series[i]->node.push_back( j-1 );
    }
    series[i]->init(configf);
  }

}

Timeseries::Timeseries( size_type nodes, double deltat, size_type index )
  : NF(nodes,deltat,index), series() {
}

Timeseries::~Timeseries() {
  for(auto & serie : series) {
    delete serie;
  }
}

void Timeseries::fire( vector<double>& Q ) const {
  vector<double> temp(nodes, 0.0);
  Q.clear();
  Q.resize(nodes, 0.0);
  for(auto serie : series) { // for each timeseries
    // if the timeseries is active
    if( (serie->t >= 0) && (serie->t < serie->duration) ) {
      temp.assign(nodes, 0.0);
      serie->fire(temp);
      // then copy the temporary firing to the final firing
      for(double j : serie->node) {
        Q[j] += temp[j];
      }
    }
  }
}

void Timeseries::step() {
  for(auto & serie : series) {
    serie->t += deltat;
  }
}

namespace TIMESERIES {

void Const::init( Configf& configf ) {
  // Mean: 0
  configf.param("Mean",mean);
}

void Const::fire( vector<double>& Q ) const {
  for( size_type i=0; i<nodes; i++ ) {
    Q[i] = mean;
  }
}

void Pulse::init( Configf& configf ) {
  period = 1000.0;
  pulses = 1;
  // Amplitude: 1 Width: .5e-3 "Period/Frequency": 1 "Pulses": 1
  configf.param("Amplitude",amp);
  configf.param("Width",width);
  if( !configf.optional("Period",period) ) {
    if( configf.optional("Frequency",period) ) {
      period = 1.0/period;
    }
  }
  configf.optional("Pulses",pulses);
}

void Pulse::fire( vector<double>& Q ) const {
  if( fmod(t,period)>=0 && fmod(t,period)<width && t/period<pulses ) {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = amp;
    }
  }
}

void White::init( Configf& configf ) {
  // Mean: 1 Std: 1 Ranseed: 1
  // Mean: 1 Psd: 1 Ranseed: 1
  configf.param("Mean",mean);
  if( !configf.optional("Std",amp) ) {
    configf.param("Psd",amp);
    // index of timeseries the same as that of population

    if(nodes>1) {
      deltax = atof(configf.find(
                      label("Population ",index+1)+"*Length").c_str()) /sqrt(nodes);
      amp = sqrt(4.0*pow(M_PI,3)*pow(amp,2)/deltat/pow(deltax,2));
    } else {
      amp = sqrt(M_PI*pow(amp,2)/deltat);
    }

  }
  if(configf.optional("Ranseed",seed)) {
    random = new Random(seed,mean,amp);
  } else {
    random = new Random(mean,amp);
  }
}

void White::fire( vector<double>& Q ) const {
  for(double& x : Q) {
    random->get(x);
  }
}

void WhiteCoherent::init( Configf& configf ) {
  // Mean: 1 Std: 1 Ranseed: 1
  // Mean: 1 Psd: 1 Ranseed: 1
  configf.param("Mean",mean);
  if( !configf.optional("Std",amp) ) {
    configf.param("Psd",amp);
    // index of timeseries the same as that of population
    double deltax = atof(configf.find(
                           label("Population ",index+1)+"*Length").c_str()) /sqrt(nodes);
    amp = sqrt(4.0*pow(M_PI,3.0)*pow(amp,2)/deltat/pow(deltax,2));
  }
  if(configf.optional("Ranseed",seed)) {
    random = new Random(seed,mean,amp);
  } else {
    random = new Random(mean,amp);
  }
}

void WhiteCoherent::fire( vector<double>& Q ) const {
  double v;
  random->get(v);
  for( double& x : Q) {
    x = v;
  }
}

void PAS::init( Configf& configf ) {
  // ISI: 10e-3
  // N20 width: 2.5e-3 N20 height: 5
  // P25 width: 3.5e-3 P25 height: 5
  // TMS width: 0.5e-3 TMS height: 3
  configf.param("ISI",isi);
  configf.param("N20 width", n20w);
  configf.param("N20 height",n20h);
  configf.param("P25 width", p25w);
  configf.param("P25 height",p25h);
  configf.param("TMS width", tmsw);
  configf.param("TMS height",tmsh);
  if( isi<0 ) {
    t -= isi;
    t_mns = -isi;
  } else {
    t_mns = 0.0;
  }
}

void PAS::fire( vector<double>& Q ) const {
  // MNS
  if( t_mns<=t && t<t_mns+n20w ) {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = -n20h*sin(M_PI*(t-t_mns)/n20w);
    }
  } else if( t_mns+n20w<=t && t<t_mns+n20w+p25w ) {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] =  p25h*sin(M_PI*(t-t_mns-n20w)/p25w);
    }
  }

  // TMS
  if( t_mns+n20w/2+isi<=t && t<t_mns+n20w/2+isi+tmsw ) {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] += tmsh;
    }
  }
}

void Burst::init( Configf& configf ) {
  // Amplitude: 10 Width: .5e-3 Bursts: 3 Burst Frequency: 50 On: 2 Off: 8 Total Pulses: 1000
  configf.param("Amplitude",amp);
  configf.param("Width",width);
  configf.param("Bursts",bursts);
  configf.param("Burst Frequency",freq);
  configf.param("Oscillation Frequency",oscillation_freq);
  configf.param("On",on);
  configf.param("Off",off);
}

void Burst::fire( vector<double>& Q ) const {
  if( fmod(t,on+off)>=0 && fmod(t,on+off)<on &&
      fmod(t,1/oscillation_freq)>=0 && fmod(t,1/oscillation_freq)<bursts/freq &&
      fmod(t,1/freq)>=0 && fmod(t,1/freq)<width ) {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = amp;
    }
  }
}

void Sine::init( Configf& configf ) {
  // Amp: 1 Width: .5 Period: 1 Phase: 0
  configf.param("Amp",amp);
  configf.param("Width",width);
  period = 1.0;
  configf.optional("Period",period);
  phase  = 0.0;
  configf.optional("Phase",phase);
  pulses = 1;
  configf.optional("Pulses",pulses);
}

void Sine::fire( vector<double>& Q ) const {
  if( fmod(t,period)>=0 && fmod(t,period)<width && t/period<pulses ) {
    for( size_type i=0; i<nodes; i++ ) {
      Q[i] = amp*sin( 2.0*M_PI*( fmod(t,period)/width -phase/360.0 ) );
    }
  }
}

} // namespace TIMESERIES
