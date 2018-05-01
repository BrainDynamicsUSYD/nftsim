/** @file solver.cpp
  @brief Solver member function definitions.

  The Solver class pulls together Population, Propagator, and Coupling classes.

  If you define your own Propagators or Couplings, then, it is in the init
  member-function of this class that you must create appropriate entries in
  order to make those Propagators and Couplings specifiable via nftsim's
  configuration (.conf) files.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "solver.h"   // Solver;

// Other nftsim headers
#include "bcm.h"           // BCM;
#include "bcmlong.h"       // BCMLong;
#include "cadp.h"          // CaDP;
#include "configf.h"       // Configf;
#include "coupling.h"      // Coupling;
#include "coupling_diff_arctan.h" // CouplingDiffArctan;
#include "coupling_ramp.h" // CouplingRamp;
#include "harmonic.h"      // Harmonic;
#include "harmonic_integral.h" // HarmonicIntegral;
#include "long_coupling.h" // LongCoupling;
#include "output.h"        // Output; Outlet;
#include "population.h"    // Population;
#include "wave.h"          // Wave;
#include "wave_legacy.h"   // WaveLegacy;

// C++ standard library headers
#include <cmath>    // std::lround; std::remainder; std::sqrt;
#include <iostream> // std::cerr; std::endl;
#include <sstream>  // std::stringstream;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::lround;
using std::modf;
using std::remainder;
using std::sqrt;
using std::string;
using std::stringstream;
using std::vector;


void Solver::CntMat::init( Configf& configf ) {
  // Read the number of populations
  configf.next("From");
  // Expect format "From: 1 2 3 4", read the
  vector<double> pop = configf.numbers();
  npop = pop.size();

  raw.resize(npop);

  for( vector<double>::size_type i=0; i<npop; i++ ) {
    configf.next( label("To ",i+1) ); // ignore "To ?:"
    raw[i] = configf.numbers();
    if( raw[i].size() != npop ) {
      cerr << "The connection matrix is not configured correctly."
           << endl;
      exit(EXIT_FAILURE);
    }
  }

  // presynaptic population index for each connection index
  for( vector<double>::size_type i=0; i<npop; i++ ) {
    for( vector<double>::size_type j=0; j<npop; j++ ) {
      if( raw[i][j] != 0.0 ) {
        pre.push_back(j);
      }
    }
  }

  // postsynaptic population index for each connection index
  for( vector<double>::size_type i=0; i<npop; i++ ) {
    for( vector<double>::size_type j=0; j<npop; j++ ) {
      if( raw[i][j] != 0.0 ) {
        post.push_back(i);
      }
    }
  }

  ncnt = post.size(); // == pre.size()
}

Solver::Solver( Dumpf& dumpf )
  : NF(0,0,0), dumpf(dumpf), cnt() {
}

Solver::~Solver() {
  delete outputs;
}

void Solver::init( Configf& configf ) {
  // Anything before the first ':' is ignored as comment

  // read in simulation time and timestep
  double tempf;
  configf.param("Time",tempf);
  configf.param("Deltat",deltat);
  if( remainder(tempf, deltat) != 0.0 ) {
    cerr << "WARNING: Value of total simulation Time not divisible by Deltat.\n"
         << "    It is STRONGLY RECOMMENDED that any times that you specify\n"
         << "    in your configuration files be exact integer multiples of\n"
         << "    Deltat. Due to the finite precision of floating-point numbers,\n"
         << "    exact multiples are only possible for machine representable\n"
         << "    values of Deltat. A simple means of achieving this is to use\n"
         << "    powers of two for Deltat (eg. 2^-13 == 1.220703125e-04)."
         << endl;
    //exit(EXIT_FAILURE);
  }

  steps = static_cast<int>(lround(tempf/deltat));

  // read in grid size and grid geometry
  configf.param("Nodes",nodes);
  size_type longside;
  if( configf.optional("Longside nodes", longside) ) {
    if( nodes%longside != 0 ) {
      cerr << "To define a rectangular grid with a total of: " << nodes << " nodes, the parameter " 
           << "Nodes divided by Longside: " << longside << endl
           << "must have no remainder" << endl;
      exit(EXIT_FAILURE);
    }
  } else {
    double intpart = 0.0; //Unfortunately required dummy variable that we will not use.
    if(modf(sqrt(nodes), &intpart) != 0.0) {
      cerr << "To define a square grid, Nodes(" << nodes << ")"
           << " must be a perfect square." << endl;
      exit(EXIT_FAILURE);
    }
    longside = static_cast<size_type>(sqrt(nodes));
  }
  string topology("Torus");
  if( configf.optional("Topology",topology) ) {
    if( topology == "Nonperiodic" ) {
      double bath;
      configf.param("Bath",bath);
      stringstream ss;
      ss<<topology<<" "<<bath;
      topology = ss.str();
    }
  }

  // read in connection matrix
  configf.param("Connection matrix",cnt);

  // construct populations
  for( vector<double>::size_type i=0; i<cnt.npop; i++ ) {
    /*{
      bool neuralpop = false; // marker of neural or Betz population
      for( vector<int>::size_type j=0; j<cnt.ncnt; j++ )
        if( cnt.pre[j] == i )
          neuralpop = true; // a Betz cell is one without postsynaptic connections
      if( neuralpop ) // neural population includes stimulus populations too*/
    pops.add( new Population(nodes,deltat,i) );
  }
  /*else // Betz cell
    pops.add( new Single(nodes,deltat,i) );
  }*/

  for( vector<int>::size_type i=0; i<cnt.ncnt; i++ ) {
    string ptype = configf.find( label("Propagator ",i+1) +":" );
    // PUT YOUR PROPAGATORS HERE
    if(ptype=="Map") {
      propagators.add( new
                   Propagator(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
    } else if(ptype=="Wave") {
      if( nodes==1 ) {
        propagators.add( new
                     Harmonic(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
      } else {
        propagators.add( new
                     Wave(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
      }
    } else if(ptype=="Harmonic") {
      propagators.add( new
                   Harmonic(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
    } else if(ptype=="HarmonicIntegral") {
      propagators.add( new
                   HarmonicIntegral(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
    } else if(ptype=="WaveLegacy") {
      if( nodes==1 ) {
        propagators.add( new
                     Harmonic(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
      } else {
        propagators.add( new
                     WaveLegacy(nodes,deltat,i, *pops[cnt.pre[i]], *pops[cnt.post[i]], longside, topology));
      }
    } else {
      cerr<<"Invalid propagator type '"<<ptype<<"'."<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR PROPAGATORS HERE

    string ctype = configf.find( label("Coupling ",i+1) +":" );
    // PUT YOUR COUPLINGS HERE
    if(ctype=="Map") {
      couplings.add( new
                   Coupling(nodes,deltat,i, *propagators[i], *pops[cnt.post[i]] ) );
    } else if(ctype=="Matrix") {
      couplings.add( new
                   LongCoupling(nodes,deltat,i, *propagators[i], *pops[cnt.post[i]] ) );
    } else if(ctype=="CaDP") {
      couplings.add( new
                   CaDP(nodes,deltat,i, *propagators[i], *pops[cnt.post[i]] ) );
    } else if(ctype=="BCM") {
      couplings.add( new
                   BCM(nodes,deltat,i, *propagators[i], *pops[cnt.post[i]] ) );
    } else if(ctype=="BCM-Spatial") {
      couplings.add( new
                   BCMLong(nodes,deltat,i, *propagators[i], *pops[cnt.post[i]] ) );
    } else if(ctype=="Ramp") {
      couplings.add( new
                   CouplingRamp(nodes,deltat,i, *propagators[i], *pops[cnt.post[i]] ) );
      //else if(ctype=="fCaP")
      //couplings.add( new
      //fCaP(nodes,deltat,i, *propagators[i], *pops[cnt.post[i]] ) );
      //else if(ctype=="Epilepsy")
      //couplings.add( new
      //Epilepsy(nodes,deltat,i, *propagators[i], *pops[cnt.post[i]] ) );
    } else if(ctype=="DiffArctan") {
      couplings.add( new
                   CouplingDiffArctan(nodes,deltat,i, *propagators[i], *pops[cnt.post[i]], tempf ) );
    } else {
      cerr<<"Invalid coupling type '"<<ctype<<"'."<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR COUPLINGS HERE
  }

  // insert dendrites to each population
  for( vector<double>::size_type i=0; i<cnt.npop; i++ ) {
    for( vector<int>::size_type j=0; j<cnt.ncnt; j++ ) {
      if( cnt.post[j] == i ) {
        pops[i]->add2Dendrite( j, *propagators[j], *couplings[j], configf );
      }
    }
  }

  // read object parameters
  for( vector<double>::size_type i=0; i<cnt.npop; i++ ) {
    configf.param( label("Population ",i+1), *pops[i] );
  }
  for( vector<int>::size_type i=0; i<cnt.ncnt; i++ ) {
    configf.param( label("Propagator ",i+1), *propagators[i] );
  }
  for( vector<int>::size_type i=0; i<cnt.ncnt; i++ ) {
    configf.param( label("Coupling ",i+1), *couplings[i] );
  }

  // initialize outputs
  configf.go2("Output");
  configf.next("Output");
  outputs = new Outputs(nodes,deltat,dumpf,cnt,pops,propagators,couplings);
  outputs->init(configf);
}

void Solver::solve() {
  for( int i=0; i<steps; i++ ) {
    step();
  }
}

void Solver::step() {
  // step through populations
  couplings.pstep();
  pops.pstep();
  propagators.pstep();
  outputs->step();
}

void Solver::Outputs::step() {
  if( outputstart != 0 ) {
    outputstart--;
  } else {
    t++;
    if( t%outputinterval==0 ) {
      dumpf<<double(t)*deltat<<space<<space<<septor;
      for(auto & outlet : outlets) {
        writeOutlet( *outlet );
      }
      dumpf<<endl;
    }
  }
}
void Solver::Outputs::init( Configf& configf ) {
  // repeat config file into output
  dumpf.outputConfig(configf);

  double tempf;
  string temps;
  vector<double> temp_node;
  // read in nodes to output
  //int position = tellg();
  if( configf.next("Node") ) {
    temp_node = configf.numbers();
  }
  if( temp_node.empty() ) {
    //seekg(position,ios::beg);
    for( size_type i=0; i<nodes; i++ ) {
      node.push_back(i);
    }
  }

  for(double i : temp_node) {
    if( i > nodes ) {
      cerr<<"Trying to plot node number "<<i
          <<", which is bigger than the highest node index."<<endl;
      exit(EXIT_FAILURE);
    } else {
      node.push_back( i-1 );
    }
  }

  // read in time to start of output
  if( !configf.optional("Start",tempf) ) {
    outputstart = 0;
  } else {
    if( remainder(tempf, deltat) != 0.0 ) {
      cerr << "WARNING: Value of output Start time not divisible by Deltat.\n"
           << "    It is STRONGLY RECOMMENDED that any times that you specify\n"
           << "    in your configuration files be exact integer multiples of\n"
           << "    Deltat. Due to the finite precision of floating-point numbers,\n"
           << "    exact multiples are only possible for machine representable\n"
           << "    values of Deltat. A simple means of achieving this is to use\n"
           << "    powers of two for Deltat (eg. 2^-13 == 1.220703125e-04)."
           << endl;
      //exit(EXIT_FAILURE);
    }
    outputstart = static_cast<int>(tempf/deltat);
  }
  t = 0;

  // read in output interval
  if( !configf.optional("Interval",tempf) ) {
    outputinterval = 1;
  } else {
    if( remainder(tempf, deltat) != 0.0 ) {
      cerr << "WARNING: Value of output Interval NOT divisible by Deltat.\n"
           << "    It is STRONGLY RECOMMENDED that any times that you specify\n"
           << "    Interval (ie, output sampling period) in your configuration files\n"
           << "    be exact integer multiples of Deltat. Due to the finite precision\n"
           << "    of floating-point numbers, exact multiples are only possible for machine representable\n"
           << "    values of Deltat. A simple means of achieving this is to use\n"
           << "    powers of two for Deltat (eg. 2^-13 == 1.220703125e-04)."
           << endl;
      //exit(EXIT_FAILURE);
    }
    outputinterval = static_cast<int>(lround(tempf/deltat));
  }

  // read in populations to output
  configf.next("Population");
  vector<string> temp = configf.arb("Dendrite:");
  for(auto & i : temp) {
    auto obj_index = static_cast<vector<double>::size_type>(atoi(i.c_str())); // atoi() takes only 1 of "1.V"
    if( obj_index > cnt.npop || obj_index<1 ) {
      cerr<<"ERROR:: INVALID OBJECT INDEX:: Trying to output Population "<<obj_index
          <<", which is an invalid Population."<<endl;
      exit(EXIT_FAILURE);
    }
    string key;
    if( i.find('.')!=string::npos ) {
      key = i.substr( i.find('.')+1, string::npos );
    }
    Output output(key);
    pops[obj_index-1]->output(output);
    if( output.empty() ) {
      cerr<<"Population "<<i.c_str()<<" cannot be written to file."<<endl;
      exit(EXIT_FAILURE);
    }
    add(output);
  }

  // read in dendrites to output
  configf.next("Dendrite");
  temp = configf.arb("Propagator:");
  for(auto & i : temp) {
    auto obj_index = static_cast<vector<double>::size_type>(atoi(i.c_str()));
    if( obj_index > cnt.ncnt || obj_index<1 ) {
      cerr<<"ERROR:: INVALID OBJECT INDEX:: Trying to output Dendrite "<<obj_index
          <<", which is an invalid Dendrite."<<endl;
      exit(EXIT_FAILURE);
    }
    string key;
    if( i.find('.')!=string::npos ) {
      key = i.substr( i.find('.')+1, string::npos );
    }
    Output output(key);
    for( size_t j=0; j<pops.size(); j++ ) {
      pops[j]->outputDendrite(obj_index-1,output);
    }
    if( output.empty() ) {
      cerr<<"Dendrite "<<i.c_str()<<" cannot be written to file."<<endl;
      exit(EXIT_FAILURE);
    }
    add(output);
  }

  // read in propagators to output
  configf.next("Propagator");
  temp = configf.arb("Coupling:");
  for(auto & i : temp) {
    auto obj_index = static_cast<vector<double>::size_type>(atoi(i.c_str()));
    if( obj_index > cnt.ncnt || obj_index<1 ) {
      cerr<<"ERROR:: INVALID OBJECT INDEX:: Trying to output Propagator "<<obj_index
          <<", which is an invalid Propagator."<<endl;
      exit(EXIT_FAILURE);
    }
    string key;
    if( i.find('.')!=string::npos ) {
      key = i.substr( i.find('.')+1, string::npos );
    }
    Output output(key);
    propagators[obj_index-1]->output(output);
    if( output.empty() ) {
      cerr<<"Propagator "<<i.c_str()<<" cannot be written to output."<<endl;
      exit(EXIT_FAILURE);
    }
    add(output);
  }

  // read in couplings to output
  configf.next("Coupling");
  // config files do not contain "EOF", reads to end of file
  // this raises the error flag in configf
  // and forbids any further reading of the config file
  temp = configf.arb("EOF");
  for(auto & i : temp) {
    auto obj_index = static_cast<vector<double>::size_type>(atoi(i.c_str()));
    if( obj_index > cnt.ncnt || obj_index<1 ) {
      cerr<<"ERROR:: INVALID OBJECT INDEX:: Trying to output Coupling "<<obj_index
          <<", which is an invalid Coupling."<<endl;
      exit(EXIT_FAILURE);
    }
    string key;
    if( i.find('.')!=string::npos ) {
      key = i.substr( i.find('.')+1, string::npos );
    }
    Output output(key);
    couplings[obj_index-1]->output(output);
    if( output.empty() ) {
      cerr<<"Coupling "<<i.c_str()<<" cannot be written to file."<<endl;
      exit(EXIT_FAILURE);
    }
    add(output);
  }

  // write out first row
  dumpf<<space<<"Time"<<space<<space<<septor;
  for(auto & outlet : outlets) {
    writeName(*outlet);
  }
  // write out second row
  dumpf<<endl<<space<<space<<space<<" "<<septor;
  for(auto & outlet : outlets) {
    writeNode(*outlet);
  }
  dumpf<<endl;
}

void Solver::Outputs::writeName( Outlet& outlet ) {
  if( outlet.single_output ) {
    dumpf<<space<<space<<setw<<outlet.name;
  } else {
    for( size_t i=0; i<node.size(); i++ ) {
      dumpf<<space<<space<<setw<<outlet.name;
    }
  }
  dumpf<<space<<space<<septor;
}

void Solver::Outputs::writeNode( Outlet& outlet ) {
  if( outlet.single_output ) {
    dumpf<<space<<space<<setw<<1;
  } else {
    for(unsigned int i : node) {
      dumpf<<space<<space<<setw<<int(i+1);
    }
  }
  dumpf<<space<<space<<septor;
}

void Solver::Outputs::writeOutlet( Outlet& outlet ) {
  if( outlet.single_output ) {
    dumpf<<space<<outlet.field[0]<<space<<space<<septor;
  } else {
    for(unsigned int i : node) {
      dumpf<<space<<outlet.field[i];
    }
    dumpf<<space<<space<<septor;
  }
}
