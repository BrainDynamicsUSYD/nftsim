#include"burst.h"
// look up qresponse.cpp for example

void BurstResponse::init( Configf& inputf )
{
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
}

BurstResponse::~BurstResponse(void)
{
}

void BurstResponse::step(void)
{
}

void BurstResponse::fire( vector<double>& Q ) const
{
}

const vector<double>& BurstResponse::V(void) const
{
}

vector<Output*> BurstResponse::output(void) const
{
  vector<Output*> temp;
  temp.push_back( new Output( label("Burst.",index+1)+".ic", ic ) ); // ic not defined yet
  return vector<Output*>(1,temp);
}
