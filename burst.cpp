#include"burst.h"
// look up qresponse.cpp for example

void BurstResponse::init( Configf& inputf )
{
 // read config file- see qresponse.init()
 // initialize any other private variables (i.e. current)
}

void BurstResponse::restart( Restartf& restartf )
{
  // leave empty
}

void BurstResponse::dump( Dumpf& dumpf ) const
{
  // leave empty
}

BurstResponse::BurstResponse( int nodes, double deltat, int index )
	: QResponse(nodes,deltat,index)
{
  // leave empty
}

BurstResponse::~BurstResponse(void)
{
  // leave empty
}

void BurstResponse::step(void)
{
  // happens once per timestep. calculates V by summing soma potentials
    dendrites.step();
  for( int i=0; i<nodes; i++ )
    v[i] = 0;
  for( size_t i=0; i<dendrites.size(); i++ )
    for( int j=0; j<nodes; j++ )
      v[j] += dendrites[i]->V()[j];
  // also perform an rk4 step to update the current variables
  this->rk4();
}

void BurstResponse::fire( vector<double>& Q ) const
{
 // map voltage into firing rate
}

const vector<double>& BurstResponse::V(void) const
{
  // leave empty if you like, it just returns v
}

vector<Output*> BurstResponse::output(void) const
{
  // write stuff into an output file
  // see cadp.cpp
  vector<Output*> temp;
  temp.push_back( new Output( label("Burst.",index+1)+".ic", ic ) ); // ic not defined yet
  return vector<Output*>(1,temp);
}

// Write an rk4 function here
