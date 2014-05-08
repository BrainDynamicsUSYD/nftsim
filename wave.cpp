#include<cmath>
#include"wave.h"
using std::cerr;
using std::endl;

void Wave::init( Configf& configf )
{
  deltax = postpop.sheetlength()/sqrt(nodes);

  string buffer("Steady");
  configf.optional("phi",buffer);
  double Q = prepop.Qinit(configf);
  if( buffer != "Steady" ) {
    p.clear();
    p.resize(nodes,atof(buffer.c_str()));
  }
  else {
    p.clear();
    p.resize(nodes,Q);
  }

  configf.optional("Tau",tau); prepop.growHistory(tau);

  configf.param("Range",range);
  if( !configf.optional("gamma",gamma) ) {
    double temp; configf.param("velocity",temp);
    gamma = temp/range;
  }

  *oldp[0] = vector<double>(nodes,p[0]);
  *oldp[1] = vector<double>(nodes,p[0]);
  *oldQ[0] = vector<double>(nodes,Q);
  *oldQ[1] = vector<double>(nodes,Q);

  dt2on12 = deltat*deltat/12.;
  dfact = dt2on12*gamma*gamma;
  dt2ondx2 = deltat*deltat/deltax/deltax;
  p2 = dt2ondx2*range*range*gamma*gamma;
  tenminus3p2 = 10.-3.*p2;
  twominus3p2 = 2.-3.*p2;
  exp1 = exp(-deltat*gamma);
  exp2 = exp(-2.*deltat*gamma);

  if( gamma/2.0 < deltat){
    cerr<<"gamma = " << gamma << " requires deltat < " << gamma/2.0 <<endl;
    exit(EXIT_FAILURE);
  }

  if(range/2.0 < deltax){
    cerr<<"range = " << range << " requires deltax < " << range/2.0 <<endl;
    exit(EXIT_FAILURE);
  }

  if( gamma*range*deltat/deltax >1.41 ) {
    cerr<<"Wave equation does not fulfill the Courant condition."<<endl;
    exit(EXIT_FAILURE);
  }
}

Wave::Wave( int nodes, double deltat, int index, Population& prepop,
        Population& postpop, int longside, string topology )
    : Propag(nodes,deltat,index,prepop,postpop,longside,topology),
        key(0)
{
  oldp[0] = new Stencil(nodes,longside,topology);
  oldp[1] = new Stencil(nodes,longside,topology);
  oldQ[0] = new Stencil(nodes,longside,topology);
  oldQ[1] = new Stencil(nodes,longside,topology);
}

Wave::~Wave(void)
{
  delete oldp[0]; delete oldp[1];
  delete oldQ[0]; delete oldQ[1];
}

void Wave::step(void)
{
  for( int i=0; i<nodes; i++,
          (*oldp[0])++, (*oldQ[0])++, (*oldp[1])++, (*oldQ[1])++ ) {
    sump    = (*oldp[key]) (n) +(*oldp[key]) (s) +(*oldp[key]) (w) +(*oldp[key]) (e);
    diagsump= (*oldp[key])(nw) +(*oldp[key])(ne) +(*oldp[key])(sw) +(*oldp[key])(se);
    sumQ    = (*oldQ[key]) (n) +(*oldQ[key]) (s) +(*oldQ[key]) (w) +(*oldQ[key]) (e);
    diagsumQ= (*oldQ[key])(nw) +(*oldQ[key])(ne) +(*oldQ[key])(sw) +(*oldQ[key])(se);
    drive = dfact*( tenminus3p2*exp1* (*oldQ[key]) +prepop.Q(tau)[i] +exp2* (*oldQ[!key]) +exp1*.5*p2*(sumQ+.5*diagsumQ) );
    p[i] = twominus3p2*exp1 *(*oldp[key]) +exp1*.5*p2*(sump+.5*diagsump) -exp2 *(*oldp[!key]) +drive;
  }

  key = !key;
  *oldp[key] = p;
  *oldQ[key] = prepop.Q(tau);
}
