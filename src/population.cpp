/** @file population.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "population.h" // Population;

// Other nftsim headers
#include "burst.h"      // BurstResponse;
#include "configf.h"    // Configf;
#include "coupling.h"   // Coupling;
#include "glutamate_response.h"  // GlutamateResponse;
#include "output.h"     // Output;
#include "propagator.h" // Propagator;
#include "firing_response.h"  // FiringResponse;
#include "tau.h"        // Tau;
#include "timeseries.h" // Timeseries;

// C++ standard library headers
#include <iostream> // std::cerr; std::endl;
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

void Population::init( Configf& configf ) {
  qinit = Qinit(configf);
  qhistory.push_back( vector<double>(nodes, qinit) );
  q.resize(nodes, qinit);

  configf.param("Length", length);

  if( firing_response != nullptr ) { // neural population
    double temp;
    configf.param("Q", temp);
    configf.param( "Firing", *firing_response );
  } else { // stimulus population
    timeseries = new Timeseries(nodes, deltat, index);
    configf.param( "Stimulus", *timeseries );
  }
  settled = true;
}

Population::Population( size_type nodes, double deltat, size_type index )
  : NF(nodes,deltat,index),
    firing_response(nullptr), timeseries(nullptr), qkey(0), settled(false) {
}

Population::~Population() {
  if(firing_response != nullptr) {
    delete firing_response;
  }
  if(timeseries != nullptr) {
    delete timeseries;
  }
}

void Population::step() {
  // move pointer to keyring
  qkey = (qkey+1) % qhistory.size();
  if( firing_response != nullptr ) { // neural population
    firing_response->step();
    firing_response->fire( qhistory[qkey] );
  } else { // stimulus population
    timeseries->step();
    timeseries->fire( qhistory[qkey] );
  }

  // q is the current Q, only for output purpose
  for( size_type i=0; i<nodes; i++ ) {
    q[i] = qhistory[qkey][i];
  }
}

double Population::Qinit( Configf& configf ) const {
  if( firing_response != nullptr ) {
    string buffer = configf.find( label("Population ",index+1)+"*Q:");
    return atof(buffer.c_str());
  }

  string temp = configf.find( label("Population ",index+1)+"*Mean:");
  return atof( temp.c_str() );
}

const vector<double>& Population::glu() const {
  if( firing_response != nullptr ) {
    auto local_qr = dynamic_cast<GlutamateResponse*>(firing_response);
    if( local_qr == nullptr ){
      cerr<<"Trying to access glu of a non-GlutatmateResponse population."<<endl;
      exit(EXIT_FAILURE);
    } else {
      return local_qr->glu();
    }
  }
  cerr<<"Trying to access glu of a stimulus population."<<endl;
  exit(EXIT_FAILURE);
}

void Population::add2Dendrite( size_type index, const Propagator& prepropag,
                               const Coupling& precouple, Configf& configf ) {
  if( settled ) {
    cerr<<"Population is already settled, no more dendrites can be added!"
        <<endl;
    exit(EXIT_FAILURE);
  }

  if( firing_response == nullptr ) {
    string temp(configf.find( label("Population ", this->index+1)+"*Firing:" ));
    // PUT YOUR FIRING_RESPONSE HERE
    if( temp == "Bursting" ) {
      firing_response = new BurstResponse(nodes, deltat, this->index);
    } else if( temp == "GlutamateResponse" ){
      firing_response = new GlutamateResponse(nodes, deltat, this->index);
      //else if( temp == "FS" )
      //firing_response = new FS(nodes,deltat,this->index);
    // END PUT YOUR FIRING_RESPONSE HERE
    } else {
      firing_response = new FiringResponse(nodes, deltat, this->index);
    }
  }
  firing_response->add2Dendrite( index, prepropag, precouple, configf );
}

const vector<double>& Population::Q( const Tau& tau) const {
  if( tau.m.size() == 1 ) { // homogeneous tau
    return qhistory[(qkey-tau.m[0]+qhistory.size())%qhistory.size()];
  }// tau.m.size() == nodes, inhomogeneous tau
  static vector<double> temp(nodes);
  for( size_type i=0; i<nodes; i++ ) {
    temp[i] = qhistory[(qkey-tau.m[i]+qhistory.size())%qhistory.size()][i];
  }
  return temp;
}

const vector<double>& Population::V() const {
  if( firing_response != nullptr ) {
    return firing_response->V();
  }
  cerr<<"Trying to access V of a stimulus population."<<endl;
  exit(EXIT_FAILURE);
}

void Population::growHistory( const Tau& tau ) {
  /*if( settled ) {
    cerr<<"Population is already settled, cannot resize firing history!"
        <<endl;
    exit(EXIT_FAILURE);
  }*/

  if( tau.max > qhistory.size() ) {
    qhistory.resize( tau.max+1 );
    for(auto & i : qhistory) {
      i.resize( nodes, qinit );
    }
  }
}

void Population::output( Output& output ) const {
  output("Pop",index+1,"Q",q);
  if(firing_response != nullptr) {
    firing_response->output(output);
  } else {
    timeseries->output(output);
  }
}

void Population::outputDendrite( size_type index, Output& output ) const {
  if(firing_response != nullptr) {
    return firing_response->outputDendrite(index,output);
  }
}
