#include"burst.h"

void BurstResponse::init( Configf& inputf )
{
}

void BurstResponse::restart( Restartf& restartf )
{
}

void BurstResponse::dump( Dumpf& dumpf ) const
{
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
