/** @file timeseries.cpp
  @brief A collection of time-series definitions, primarily for use as stimuli.

  The currently implemented time-series include:
    + TIMESERIES::Const: A constant value.
    + TIMESERIES::PulseRect: A rectangular pulse train.
    + TIMESERIES::White: Gaussian amplitude distribution white noise.
    + TIMESERIES::WhiteCoherent: .
    + TIMESERIES::PAS: Paired Associative Stimulation.
    + TIMESERIES::Burst: .
    + TIMESERIES::Sine: A sine wave.

    Multiple `Timeseries` can be combined using the `Superimpose` keyword in
    the configuration file.

  @author Peter Drysdale, Felix Fung, Stuart A. Knock,
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
    t = -onset; // Initialise stimulus time relative to onset.

    // Set stimulus duration.
    if(!configf.optional("Duration", duration) ) {
      duration = inf; // Unspecified => whole simulation.
    }

    // Get any user specified Node indices to apply stimulus.
    vector<double> temp_node;
    //TODO: should be vector<size_type>.
    //      The existing use of double is due to a limitation of the
    //      Configf which has only one member function, numbers(), for
    //      reading multiple numbers. This member returns a vector of
    //      doubles. This should be corrected by renaming numbers() to
    //      something like read_doubles() and adding a new member
    //      function for reading and returning unsigned int like size_type.
    //      Only two should be required as indices(size_type) and doubles
    //      are the only type of content we specify in vectors...
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
    } else if( mode[0]=="White" ) {
      series.push_back( new TIMESERIES::White(nodes, deltat, index) );
    } else if( mode[0]=="WhiteCoherent" ) {
      series.push_back( new TIMESERIES::WhiteCoherent(nodes, deltat, index) );
    } else if( mode[0]=="PAS" ) {
      series.push_back( new TIMESERIES::PAS(nodes, deltat, index) );
    } else if( mode[0]=="Burst" ) {
      series.push_back( new TIMESERIES::Burst(nodes, deltat, index) );
    } else if( mode[0]=="Sine" ) {
      series.push_back( new TIMESERIES::Sine(nodes, deltat, index) );
    } else {
      cerr<<"Stimulus mode "<<mode[0].c_str()<<" not found"<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR TIMEFUNCTION HERE
    series[i]->t = t;
    series[i]->duration = duration;
    for(size_type j : temp_node) {
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
  Q.assign(nodes, 0.0); // Zero the Q vector we were provided.
  for(auto serie : series) { // for each timeseries
    // if the timeseries is active
    if( (serie->t >= 0) && (serie->t < serie->duration) ) {
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

//TODO: The replication of timeseries to a vector length nodes for Q is
//      redundant when there is no explicit space to them. A single number
//      would be all that needs to be returned with mapping to nodes done
//      in Timeseries::fire(). The exception is for noise, where we want a
//      different random sequence at each point in space and typically it
//      is applied to all nodes anyway. To maintain a consistent interface,
//      it is probably best to have the vector returned by the individual
//      TIMESERIES fire() methods be of length the number of nodes that the
//      stimulus will be applied to... This becomes more important for higher
//      resolution surfaces, where currently applying a stimulus to a single
//      node of a 1000x1000 surface will produce a million element vector,
//      rather than the single number that is required...

//TODO: Naming should be more specific. Eg., current "Pulse" should probably be
//      something like "PulseRect" while current "Sine" should be something
//      like "PulseSine". A basic "Sine" should also be added which just has
//      amplitude, frequency|period, and phase with the more "usual" meanings.
//      Then we should add a more physically realistic, and less numerically
//      problematic, smooth pulse with eg sigmoidal onset and cessation, which
//      would then be "PulseSigmoid". Example parameters with associated plots
//      should be added to the user manual.

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
      amplitude: 1.0 Width: 0.5e-3
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
    if((fmod(t, period) < width) && (t/period < pulses) ) {
      Q.assign(nodes, amp); // assign nodes instances of amp to Q.
    }
  }

// TODO: get reference and review noise normalisation for White*, seems like
//       it will at best only work for square grid... need, at least, access
//       to longside here.

  /** @brief Initialises white noise.*/
  void White::init( Configf& configf ) {
    // Mean: 1 Std: 1 Ranseed: 1
    // Mean: 1 Psd: 1 Ranseed: 1
    configf.param("Mean", mean);
    if( !configf.optional("Std", amp) ) {
      configf.param("Psd", amp);
      // index of timeseries the same as that of population

      if(nodes>1) {
        deltax = atof(configf.find(
                        label("Population ",index+1)+"*Length").c_str()) /sqrt(nodes);
        amp = sqrt(4.0*pow(M_PI,3)*pow(amp,2)/deltat/pow(deltax,2));
      } else {
        amp = sqrt(M_PI*pow(amp,2)/deltat);
      }

    }
    if(configf.optional("Ranseed", seed)) {
      random = new Random(seed, mean, amp);
    } else {
      random = new Random(mean, amp);
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
    // Mean: 1 Std: 1 Ranseed: 1
    // Mean: 1 Psd: 1 Ranseed: 1
    configf.param("Mean", mean);
    if( !configf.optional("Std", amp) ) {
      configf.param("Psd", amp);
      // index of timeseries the same as that of population
      double deltax = atof(configf.find(
                             label("Population ",index+1)+"*Length").c_str()) /sqrt(nodes);
      amp = sqrt(4.0*pow(M_PI,3.0)*pow(amp,2)/deltat/pow(deltax,2));
    }
    if(configf.optional("Ranseed", seed)) {
      random = new Random(seed, mean, amp);
    } else {
      random = new Random(mean, amp);
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


  void Sine::init( Configf& configf ) {
    // Amp: 1 Width: .5 Period: 1 Phase: 0
    configf.param("Amp", amp);
    configf.param("Width", width);
    period = 1.0;
    configf.optional("Period", period);
    phase  = 0.0;
    configf.optional("Phase", phase);
    pulses = 1;
    configf.optional("Pulses", pulses);
  }

  void Sine::fire( vector<double>& Q ) const {
    if( fmod(t,period)>=0 && fmod(t,period)<width && t/period<pulses ) {
      for( size_type i=0; i<nodes; i++ ) {
        Q[i] = amp*sin( 2.0*M_PI*( fmod(t,period)/width -phase/360.0 ) );
      }
    }
  }

} // namespace TIMESERIES
