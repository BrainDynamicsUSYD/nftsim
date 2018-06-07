/** @file timeseries.cpp
  @brief A collection of time-series definitions, primarily for use as stimuli.

  The currently implemented time-series include:
    + TIMESERIES::Const: A constant value.
    + TIMESERIES::PulseRect: A rectangular pulse train.
    + TIMESERIES::PulseSine: A pulsed sine wave.
    + TIMESERIES::PulseSigmoid: A sigmoidal pulse train.
    + TIMESERIES::White: Gaussian amplitude distribution white noise.
    + TIMESERIES::WhiteCoherent: .
    + TIMESERIES::PAS: Paired Associative Stimulation.
    + TIMESERIES::Burst: .

    Multiple `Timeseries` can be combined using the `Superimpose` keyword in
    the configuration file.

  @author Peter Drysdale, Felix Fung, Stuart A. Knock,
*/

// Main module header
#include "timeseries.h" // Timeseries; TIMESERIES::;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "random.h"     // Random;

// C++ standard library headers
#include <algorithm>  // std::min; std::max
#include <cmath>    // std::pow; std::sqrt; std::sin; std::fmod; M_PI; std::ceil; std::exp
#include <iostream> // std::cerr; std::endl;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::ceil;
using std::exp;
using std::endl;
using std::fmod;
using std::max;
using std::min;
using std::pow;
using std::sin;
using std::sqrt;
using std::string;
using std::vector;

void Timeseries::init( Configf& configf ) {
  series_size_type superimpose = 1;
  configf.optional("Superimpose", superimpose);
  for( series_size_type i=0; i<superimpose; i++ ) {
    if( superimpose > 0 ) {
      configf.next("Stimulus");
    }
    vector<string> mode = configf.arb("-");

    // Set stimulus onset time.
    if(!configf.optional("Onset", onset)){
      onset = 0.0;
    }

    // Set stimulus duration.
    if(!configf.optional("Duration", duration) ) {
      duration = inf; // Unspecified => whole simulation.
    }

    // Get any user specified Node indices to apply stimulus.
    vector<double> temp_node; //TODO(stuart-knock): Should be vector<size_type>, see issue #142 on GitHub.
    if( configf.next("Node") ) {
      temp_node = configf.numbers();
    }

    // Check user supplied Node indices are valid.
    for(size_type j : temp_node) {
      if( j > nodes ) {
        cerr<<"Trying to plot node number "<<j
            <<", which is bigger than the highest node index."<<endl;
        exit(EXIT_FAILURE);
      }
    }

    // No Node indices specified in configuration defaults to all nodes.
    if( temp_node.empty() ) {
      temp_node.reserve(nodes); // Reserve memory to avoid growing inside loop.
      for( size_type j=1; j<=nodes; j++ ) {
        temp_node.push_back(j);
      }
    }

    // PUT YOUR TIMEFUNCTION HERE
    if( mode[0]=="Const" ) {
      series.push_back( new TIMESERIES::Const(nodes, deltat, index) );
    } else if( mode[0]=="PulseRect" ) {
      series.push_back( new TIMESERIES::PulseRect(nodes, deltat, index) );
    } else if( mode[0]=="PulseSine" ) {
      series.push_back( new TIMESERIES::PulseSine(nodes, deltat, index) );
    } else if( mode[0]=="PulseSigmoid" ) {
      series.push_back( new TIMESERIES::PulseSigmoid(nodes, deltat, index) );
    } else if( mode[0]=="Sine" ) {
      series.push_back( new TIMESERIES::Sine(nodes, deltat, index) );
    } else if( mode[0]=="White" ) {
      series.push_back( new TIMESERIES::White(nodes, deltat, index) );
    } else if( mode[0]=="WhiteCoherent" ) {
      series.push_back( new TIMESERIES::WhiteCoherent(nodes, deltat, index) );
    } else if( mode[0]=="PAS" ) {
      series.push_back( new TIMESERIES::PAS(nodes, deltat, index) );
    } else if( mode[0]=="Burst" ) {
      series.push_back( new TIMESERIES::Burst(nodes, deltat, index) );
    } else {
      cerr<<"Stimulus mode "<<mode[0].c_str()<<" not found"<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR TIMEFUNCTION HERE
    series[i]->onset = onset;
    series[i]->t = -onset; // Initialise stimulus time relative to onset.
    series[i]->duration = duration;
    for(size_type j : temp_node) {
      series[i]->node.push_back( j-1 );
    }
    series[i]->init(configf);
  }

}

Timeseries::Timeseries( size_type nodes, double deltat, size_type index )
  : NF(nodes,deltat,index) {
}

Timeseries::~Timeseries() {
  for(auto & serie : series) {
    delete serie;
  }
}

void Timeseries::fire( vector<double>& Q ) const {
  vector<double> temp(nodes, 0.0);
  Q.assign(nodes, 0.0); // Zero the Q vector we were provided.
  for(auto serie : series) { // for each timeseries
    // if the timeseries is active
    if( (serie->t >= 0.0) && (serie->t < serie->duration) ) {
      temp.assign(nodes, 0.0); // re-zero temp vector.
      serie->fire(temp);
      // then copy the temporary firing to the final firing
      for(size_type j : serie->node) {
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


/** @brief Contains time-series that can be combined to form a stimulus.

  **NOTE:** Timeseries::fire handles Onset and Duration.
*/
namespace TIMESERIES {

  /** @brief Initialise the value returned by Const from the .conf file.*/
  void Const::init( Configf& configf ) {
    configf.param("Mean", mean);
  }

  /** @brief Provide a constant value.*/
  void Const::fire( vector<double>& Q ) const {
    Q.assign(nodes, mean); // assign nodes instances of mean to Q.
  }


  /** @brief Parameter initialisation of a rectangular Pulse train.

    The .conf file is required to specify Amplitude and Width, eg:
      Amplitude: 1.0 Width: 0.5e-3
    with Period, Frequency, and Pulses being optional. Period and Frequency are
    mutually exclusive with Period taking precedence if both are specified.
  */
  void PulseRect::init( Configf& configf ) {
    // Set default values for optional parameters.
    period = inf;
    pulses = 1;
    configf.param("Amplitude", amp);
    configf.param("Width", width);
    if( !configf.optional("Period", period) ) {
      double freq;
      if( configf.optional("Frequency", freq) ) {
        period = 1.0/freq;
      }
    }
    configf.optional("Pulses", pulses);
  }

  /** @brief Generate a train of rectangular pulses.*/
  void PulseRect::fire( vector<double>& Q ) const {
    //  Between start of each pulse and start plus width && as long as we have
    //  not reached the maximum number of pulses.
    if((fmod(t, period) <= width) && (t/period < pulses) ) {
      Q.assign(nodes, amp); // assign nodes instances of amp to Q.
    }
  }


  /** @brief Initialises a pulsed sine wave.

    The .conf file is required to specify Amplitude and Width, eg:
      Amplitude: 1.0 Width: 0.5e-3
    with Period, phase, and Pulses being optional.
  */
  void PulseSine::init( Configf& configf ) {
    // Amplitude: 1 Width: .5 Period: 1 Phase: 0
    configf.param("Amplitude", amp);
    configf.param("Width", width);
    period = 1.0;
    configf.optional("Period", period);
    phase  = 0.0;
    configf.optional("Phase", phase);
    pulses = 1;
    configf.optional("Pulses", pulses);
  }

  /** @brief Generate a train of sine wave pulses.*/
  void PulseSine::fire( vector<double>& Q ) const {
    if( fmod(t,period)>=0 && fmod(t,period)<width && t/period<pulses ) {
      for( size_type i=0; i<nodes; i++ ) {
        Q[i] = amp*sin( 2.0*M_PI*( fmod(t,period)/width -phase/360.0 ) );
      }
    }
  }


  /** @brief Parameter initialisation of a sigmoidal Pulse train.

    The .conf file is required to specify Amplitude and Width, eg:
      Amplitude: 1.0 Width: 0.5e-3
    with Period, Frequency, and Pulses being optional. Period and Frequency are
    mutually exclusive with Period taking precedence if both are specified.
    Finally, Sigma can be specified to adjust the width of the transition from
    zero to Amplitude.

    NOTE: Unlike other Timeseries, onset here is taken to be the mid-point of
          the onset of the first pulse. That is, the initial rise begins before
          onset. This is to make PulseSigmoid and PulseRect equivalent in the
          limit as sigma approaches zero.
  */
  void PulseSigmoid::init( Configf& configf ) {
    // Set default values for optional parameters.
    period = inf; // Time between the start of each pulse [s].
    pulses = 1.0; // Maximum number of pulses.
    configf.param("Amplitude", amp);
    configf.param("Width", width);
    if( !configf.optional("Period", period) ) {
      double freq;
      if( configf.optional("Frequency", freq) ) {
        period = 1.0 / freq;
      }
    }

    configf.optional("Pulses", pulses);

    if (!configf.optional("Sigma", sigma)){
      // Set an adaptive default width of transition. A sigmoidal-pulse
      // waveform is achieved when sigma is a small fraction of width.
      // However, to be represented properly it must be at least deltat.
      sigma = max(width / 16.0, deltat);
    }

    if( (duration != inf) && (period != inf) ) {
      pulse_count = min(pulses, duration/period);
    } else {
      pulse_count = pulses;
    }

    if( (pulse_count > 1) && (period == inf) ) {
      cerr << "ERROR: Multiple PulseSigmoid pulses requested but no Period or Frequency specified."
           << endl;
      exit(EXIT_FAILURE);
    }

    first_pulse_mid = onset + (width / 2.0);

    onset_midpoints.assign(pulse_count, onset);
    if( pulse_count > 1 ) {
      for( size_type i=1; i<pulse_count; ++i ) {
        onset_midpoints[i] = onset + (i * period);
      }
    }

    // As we need to begin evaluation before onset for this Timeseries, as
    // onset is taken here to be the mid-point of the rise of the first pulse,
    // we reset the time-series' internal time to zero and adjust the duration
    // accordingly.
    t = 0.0;
    if( duration != inf ) {
      duration = duration + onset;
    }
  }

  /** @brief Generate a train of sigmoidal pulses.*/
  void PulseSigmoid::fire( vector<double>& Q ) const {
    double tsy = 0.0; // Value of the Timeseries at the current time-point.
    size_type p1; // index of the first active pulse.
    size_type p2; // index of the second active pulse.

    // At each point in time after the mid-point of the first pulse, the two
    // nearest pulses contribute to the returned value.
    p2 = static_cast<size_type>(max(ceil((t - first_pulse_mid) / period), 1.0));
    p1 = p2 - static_cast<size_type>(1);
    
    if (p1 < pulse_count){
      tsy = amp / ((1.0 + exp(-pi_on_sqrt3 * (t-onset_midpoints[p1])/sigma)) *
                   ((1.0 + exp(-pi_on_sqrt3 * ((onset_midpoints[p1]+width)-t)/sigma))));
    }

    if (p2 < pulse_count){
        tsy += amp / ((1.0 + exp(-pi_on_sqrt3 * (t-onset_midpoints[p2])/sigma)) *
                      ((1.0 + exp(-pi_on_sqrt3 * ((onset_midpoints[p2]+width)-t)/sigma))));
    }

    Q.assign(nodes, tsy); // assign nodes instances to Q.
  }

  /** @brief Parameter initialisation of a simple sine function.

    The .conf file is required to specify Amplitude and frequency eg:
      Amplitude: 512.0 Frequency: 4.0
  */
  void Sine::init( Configf& configf ) {
    configf.param("Amplitude", amp);
    configf.param("Frequency", freq);
  }

  /** @brief Generate a sine wave.*/
  void Sine::fire( vector<double>& Q ) const {
    // Assign nodes instances of the sine wave to Q.
    Q.assign(nodes, amp*sin( 2.0 * M_PI * t * freq));
  }

  /** @brief Initialises white noise.*/
  void White::init( Configf& configf ) {
    // Mean: 1 StdDev: 1 Ranseed: 1
    // Mean: 1 ASD: 1e-05 Ranseed: 1
    // ASD stands for Amplitude Spectral Density, which is the square root of PSD (Power Spectral Density)
    configf.param("Mean", mean);
    if( !configf.optional("StdDev", stddev) ) {
      configf.param("ASD", asd);
      // index of timeseries the same as that of population

      if(nodes>1) {
        deltax = atof(configf.find(
                        label("Population ",index+1)+"*Length").c_str()) /sqrt(nodes);
        stddev = sqrt(2.0*4.0*pow(M_PI,3)*pow(asd,2)/deltat/pow(deltax,2));
      } else {
        stddev = sqrt(2.0*M_PI*pow(asd,2)/deltat);
      }

    }
    if(configf.optional("Ranseed", seed)) {
      random = new Random(seed, mean, stddev);
    } else {
      random = new Random(mean, stddev);
    }
  }

  /** @brief Retrieves a different random variate for each node(space).*/
  void White::fire( vector<double>& Q ) const {
    for(double& x : Q) {
      random->get(x);
    }
  }


  /** @brief Initialises spatially uniform/coherent white noise.*/
  void WhiteCoherent::init( Configf& configf ) {
    // Mean: 1 StdDev: 1 Ranseed: 1
    // Mean: 1 ASD: 1e-05 1 Ranseed: 1
    configf.param("Mean", mean);
    if( !configf.optional("StdDev", stddev) ) {
      configf.param("ASD", asd);
      // index of timeseries the same as that of population
      double deltax = atof(configf.find(
                             label("Population ",index+1)+"*Length").c_str()) /sqrt(nodes);
      stddev = sqrt(2.0*4.0*pow(M_PI,3.0)*pow(asd,2)/deltat/pow(deltax,2));
    }
    if(configf.optional("Ranseed", seed)) {
      random = new Random(seed, mean, stddev);
    } else {
      random = new Random(mean, stddev);
    }
  }

  /** @brief Retrieves a single random variate and assigns it to all nodes(space).*/
  void WhiteCoherent::fire( vector<double>& Q ) const {
    double v;
    random->get(v); // get a single random value for this time-step.
    Q.assign(nodes, v); // assign nodes instances of v to Q.
  }

  /** @brief Parameter initialisation of Paired Associative Stimulation (PAS).

    The .conf file is required to specify all parameters, example values are:
      ISI: 10.0e-3
      N20 width: 2.5e-3  N20 height: 5.0
      P25 width: 3.5e-3  P25 height: 5.0
      TMS width: 0.5e-3  TMS height: 3.0
  */
  void PAS::init( Configf& configf ) {
    // Load parameter values from the conf file.
    configf.param("ISI", isi);
    configf.param("N20 width",  n20w);
    configf.param("N20 height", n20h);
    configf.param("P25 width",  p25w);
    configf.param("P25 height", p25h);
    configf.param("TMS width",  tmsw);
    configf.param("TMS height", tmsh);
    // Adjust time... negative ISI...???
    if( isi<0 ) {
      t -= isi;
      t_mns = -isi;
    } else {
      t_mns = 0.0;
    }
  }

  /** @brief Paired Associative Stimulation (PAS).

    PAS is a Transcranial Magnetic Stimulation (TMS) protocol that consists
    of electrical stimulation of the median nerve at the wrist (MNS) followed
    by TMS of the contralateral primary motor region (M1).

    Wolters, Alexander, et al. "Timing‐dependent plasticity in human primary
    somatosensory cortex." The Journal of physiology 565.3 (2005): 1039-1052.

    Müller-Dahlhaus, Florian, Ulf Ziemann, and Joseph Classen. "Plasticity
    resembling spike-timing dependent synaptic plasticity: the evidence in
    human cortex." Frontiers in synaptic neuroscience 2 (2010).
  */
  void PAS::fire( vector<double>& Q ) const {
    // Median Nerve-evoked SomatoSensory-Evoked Potential (MN-SSEP).
    double amp;
    if( t_mns<=t && t<t_mns+n20w ) {
      amp = -n20h*sin(M_PI*(t-t_mns)/n20w);
      Q.assign(nodes, amp); // assign nodes instances of amp to Q.
    } else if( t_mns+n20w<=t && t<t_mns+n20w+p25w ) {
      amp = p25h*sin(M_PI*(t-t_mns-n20w)/p25w);
      Q.assign(nodes, amp); // assign nodes instances of amp to Q.
    }

    // Transcranial Magnetic Stimulation (TMS)
    if( t_mns+n20w/2+isi<=t && t<t_mns+n20w/2+isi+tmsw ) {
      for( size_type i=0; i<nodes; i++ ) {
        Q[i] += tmsh;
      }
    }
  }


  void Burst::init( Configf& configf ) {
    // Amplitude: 10 Width: .5e-3 Bursts: 3 Burst Frequency: 50 On: 2 Off: 8 Total Pulses: 1000
    configf.param("Amplitude", amp);
    configf.param("Width", width);
    configf.param("Bursts", bursts);
    configf.param("Burst Frequency", freq);
    configf.param("Oscillation Frequency", oscillation_freq);
    configf.param("On", on);
    configf.param("Off", off);
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

} // namespace TIMESERIES
