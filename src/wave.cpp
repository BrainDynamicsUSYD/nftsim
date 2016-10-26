/**
 * @file wave.cpp
 * @brief The wave propagator, including the method to step forward in time.
 *
 * @author Peter Drysdale, Felix Fung, Romesh Abeysuriya, Paula Sanz-Leon
 *
 */

/**
   Reads from the configuration file

*/
#include<cmath>
#include"wave.h"
using std::cerr;
using std::endl;

void Wave::init( Configf& configf ) {
  deltax = prepop.sheetlength()/longside;
  string buffer("Steady");
  configf.optional("phi",buffer);
  double Q = prepop.Qinit(configf);
  if( buffer != "Steady" ) {
    p.clear();
    p.resize(nodes,atof(buffer.c_str()));
  } else {
    p.clear();
    p.resize(nodes,Q);
  }

  configf.optional("Tau",tau);
  prepop.growHistory(tau);
  configf.param("Range",range);
  if( !configf.optional("gamma",gamma) ) {
    double temp;
    configf.param("velocity",temp);
    gamma = temp/range;
  }

  *oldp[0] = vector<double>(nodes,p[0]);
  *oldp[1] = vector<double>(nodes,p[0]);
  *oldQ[0] = vector<double>(nodes,Q);
  *oldQ[1] = vector<double>(nodes,Q);

  dt2on12 = deltat*deltat/12.;
  dfact = dt2on12*gamma*gamma;
  dt2ondx2 = (deltat*deltat)/(deltax*deltax);
  p2 = dt2ondx2*range*range*gamma*gamma;
  tenminus3p2 = 10.0-3.*p2;
  twominus3p2 =  2.0-3.*p2;
  expfact1 = exp(-1.0*deltat*gamma);
  expfact2 = exp(-2.0*deltat*gamma);

  if(gamma/2.0 < deltat || range/2.0 < deltax) {
    cerr << "Wave equation with gamma: " << gamma << " and range: " << range << endl;
    cerr << "is neither adequately captured by grid spacing chosen" << endl;
    cerr << "nor sufficiently localized so the potential can be approximated by Q" << endl;
    cerr << "gamma = " << gamma << " requires deltat < " << gamma/2.0 << endl;
    cerr << "and range = " << range << " requires deltax < " << range/2.0 << endl;
    exit(EXIT_FAILURE);
  }

  if( gamma*range*deltat/deltax > 1/sqrt(2.0)) {
    cerr << "Wave equation with gamma: " << gamma << " axonal range: " << range << endl;
    cerr << "and deltat: " << deltat << " deltax: " << deltax << endl;
    cerr << "does not fulfill the Courant condition" << endl;
    cerr << "Courant number is: " << (gamma*range*deltat/deltax) << endl;
    cerr << "and should be less than " << 1/sqrt(2.0) << endl;
    exit(EXIT_FAILURE);
  }
}

Wave::Wave( int nodes, double deltat, int index, Population& prepop,
            Population& postpop, int longside, string topology )
  : Propagator(nodes,deltat,index,prepop,postpop,longside,topology),
    key(0) {
  oldp[0] = new Stencil(nodes,longside,topology);
  oldp[1] = new Stencil(nodes,longside,topology);
  oldQ[0] = new Stencil(nodes,longside,topology);
  oldQ[1] = new Stencil(nodes,longside,topology);
}

Wave::~Wave() {
  delete oldp[0];
  delete oldp[1];
  delete oldQ[0];
  delete oldQ[1];
}

void Wave::step() {
  Stencil& stencilp = *oldp[key];
  Stencil& stencil_oldp = *oldp[key == 0];
  Stencil& stencilQ = *oldQ[key];
  Stencil& stencil_oldQ = *oldQ[key == 0];
  for( int i=0; i<nodes; i++,
       stencilp++, stencilQ++, stencil_oldp++, stencil_oldQ++ ) {
    sump     = stencilp(n)  +stencilp(s)  +stencilp(w)  +stencilp(e) ; // sum of the von Neumann (orthogonal) neighbourhood (phi)
    diagsump = stencilp(nw) +stencilp(ne) +stencilp(sw) +stencilp(se); // sum of the diagonal neighbourhood (phi)
    sumQ     = stencilQ(n)  +stencilQ(s)  +stencilQ(w)  +stencilQ(e) ; // sum of the von Neumann (orthogonal) neighbourhood (Q)
    diagsumQ = stencilQ(nw) +stencilQ(ne) +stencilQ(sw) +stencilQ(se); // sum of the diagonal neighbourhood (Q)
    drive    = dfact*( tenminus3p2*expfact1* stencilQ +prepop.Q(tau)[i]
                       +expfact2* stencil_oldQ +expfact1*.5*p2*(sumQ+.5*diagsumQ) );
    p[i]     = twominus3p2*expfact1 *stencilp +expfact1*.5*p2*(sump+.5*diagsump)
               -expfact2 *stencil_oldp +drive;
  }

  key = static_cast<int>(key == 0);
  //*oldp[key] = p;
  //*oldQ[key] = prepop.Q(tau);
  stencil_oldp = p;
  stencil_oldQ = prepop.Q(tau);
}
