/** @file propagator.cpp
  @brief Propagator computes the axonal propagation of instantaneous firing rate.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#include "propagator.h"

void Propagator::init( Configf& configf ) {
  Q = prepop.Qinit(configf);
  string buffer("Steady");
  configf.optional("phi",buffer);
  if( buffer != "Steady" ) {
    Q = atof(buffer.c_str());
  }
  p.clear();
  p.resize(nodes,Q);
  configf.optional("Tau",tau);
  prepop.growHistory(tau);

  // Range is either used when velocity is specified, or
  // not used but read for compatibility with Wave propagator.
  configf.optional("Range",range);
  if( !configf.optional("gamma",gamma) ) {
    if( configf.optional("velocity",velocity) ) {
      gamma = velocity/range;
    }
  }
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
