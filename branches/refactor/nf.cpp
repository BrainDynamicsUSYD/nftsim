/***************************************************************************
                          nf.cpp  -  abstract base class to specify interface
                             -------------------
    copyright            : (C) 2010 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include"nf.h"

Configf& operator>> ( Configf& configf, NF& nf )
{
  nf.init(configf);
  return configf;
}

Restartf& operator>> ( Restartf& restartf, NF& nf )
{
  nf.restart(restartf);
  return restartf;
}

Dumpf& operator<< ( Dumpf& dumpf, const NF& nf )
{
  nf.dump(dumpf);
  return dumpf;
}

NF::NF(void)
{
}

NF::~NF(void)
{
}
