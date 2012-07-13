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

vector<Output*> Couple::output(void) const
{
  Output* temp = new Output( label("Couple.",index+1)+".nu", n );
  return vector<Output*>(1,temp);
}

const vector<double>& Couple::nu(void) const
{
  return n;
}

const double Couple::operator[]( int node ) const
{
  return n[node];
}

bool Couple::excite(void) const
{
  return pos==1;
}
