#include"cmap.h"

void CMap::init( Configf& configf )
{
  if( nodes>1 )
    Wave::init(configf);
  else
    Harmonic::init(configf);
  configf.param("Electrode",edelay);

  for( int i=0; i<int(edelay); i++ )
    phistory.push_back(p);
}

void CMap::restart( Restartf& restartf )
{
}

void CMap::dump( Dumpf& dumpf ) const
{
}

CMap::CMap( int nodes, double deltat, int index,
    Population& prepop, Population& postpop, int longside, string topology )
    : Propag(nodes,deltat,index,prepop,postpop,longside,topology),
        Wave(nodes,deltat,index,prepop,postpop,longside,topology),
    Harmonic(nodes,deltat,index,prepop,postpop,longside,topology),
      edelay(nodes,deltat,index), pkey(0)
{
}

CMap::~CMap(void)
{
}

void CMap::step(void)
{
  // calculate present wave
  if( nodes>1 )
    Wave::step();
  else
    Harmonic::step();

  //int oldkey = pkey;
  pkey = (pkey+1) % phistory.size();
  phistory[pkey] = p;

  for( int i=0; i<nodes; i++ )
    p[i] -= phistory[(pkey+1)%phistory.size()][i];
}
