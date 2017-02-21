/** @file propagator.cpp
  @brief Propagator computes the axonal propagation of instantaneous firing rate.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "propagator.h" // Propagator;

// Other neurofield headers
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "population.h" // Population;

// C++ standard library headers
#include <string>   // std::string;
#include <vector>   // std::vector;
using std::string;
using std::vector;

void Propagator::init( Configf& configf ) {
  double Q = prepop.Qinit(configf);
  string buffer("Steady");
  configf.optional("phi",buffer);
  if( buffer != "Steady" ) {
    Q = atof(buffer.c_str());
  }
  p.clear();
  p.resize(nodes,Q);
  configf.optional("Tau",tau);
  prepop.growHistory(tau);
}

Propagator::Propagator( size_type nodes, double deltat, size_type index, Population& prepop,
                Population& postpop, int longside, string  /*unused*/)
  : NF(nodes,deltat,index), prepop(prepop), postpop(postpop),
    tau(nodes,deltat,index), longside(longside), p(nodes) {
}

Propagator::~Propagator() = default;

void Propagator::step() {
  p = prepop.Q(tau);
}

double Propagator::phiinit( Configf& configf ) const {
  return prepop.Qinit(configf);
}

const vector<double>& Propagator::phi() const {
  return p;
}

void Propagator::output( Output& output ) const {
  output("Propagator",index+1,"phi",p);
}
