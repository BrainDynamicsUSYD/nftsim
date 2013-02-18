#include "couple.h"

void Couple::init( Configf& configf )
{
  double nuinit; configf.param("nu",nuinit);
  n.resize(nodes,nuinit);
  pos = (nuinit>0)?1:-1;
}

void Couple::restart( Restartf& restartf )
{
}

void Couple::dump( Dumpf& dumpf ) const
{
}

Couple::Couple( int nodes, double deltat, int index, const vector<double>& glu,
        const Propag& prepropag, const Population& postpop )
    : NF(nodes,deltat,index), glu(glu), prepropag(prepropag), postpop(postpop)
{
}

Couple::~Couple(void)
{
}

void Couple::step(void)
{
}

double Couple::nuinit( Configf& configf ) const
{
  string buffer = configf.find( label("Couple ",index+1)+"*nu:");
  return atof(buffer.c_str());
}

void Couple::output( Output& output ) const
{
  output("Couple",index+1,"nu",n);
}

const vector<double>& Couple::nu(void) const
{
  return n;
}

const double Couple::operator[]( int node ) const
{
  return nu()[node];
}

bool Couple::excite(void) const
{
  return pos==1;
}
