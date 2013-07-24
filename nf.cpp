#include"configf.h"
#include"nf.h"

Configf& operator>> ( Configf& configf, NF& nf )
{
  nf.init(configf);
  return configf;
}

/*Restartf& operator>> ( Restartf& restartf, NF& nf )
{
  //nf.init( restartf );
  nf.restart(restartf);
  return restartf;
}

Dumpf& operator<< ( Dumpf& dumpf, const NF& nf )
{
  nf.dump(dumpf);
  return dumpf;
}*/

NF::NF( int nodes, double deltat, int index )
    : nodes(nodes), deltat(deltat), index(index)
{
}

void NF::output( Output&  ) const
{
}

NF::~NF(void)
{
}
