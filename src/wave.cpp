/**
 * @file wave.cpp
 * @brief The wave propagator, including the method to step forward in time.
 *
 * @author Peter Drysdale, Felix Fung, Romesh Abeysuriya, Paula Sanz-Leon
 *
 */

// Main module header
#include "wave.h"       // Wave;

// Other neurofield headers
#include "configf.h"    // Configf;
#include "population.h" // Population;
#include "stencil.h"    // Stencil;

// C++ standard library headers
#include <cmath>     // std::exp; std::sqrt;
#include <iostream>  // std::cerr; std::endl;
#include <string>    // std::string;
#include <vector>    // std::vector;
using std::cerr;
using std::endl;
using std::exp;
using std::sqrt;
using std::string;
using std::vector;

void Wave::init( Configf& configf ) {
  Propagator::init(configf);

  // Propagator::init considers range optional, but for Wave it's required.
  if(range == 0.0) {
    cerr << "Wave requires Range to be set in the .conf file." << endl;
    exit(EXIT_FAILURE);
  }

  // Propagator::init considers gamma optional but Wave requires gamma, so
  // either it or velocity must be set in the .conf file (gamma=velocity/range).
  if(gamma == 0.0) {
    cerr << "Wave requires either gamma or velocity to be set in the .conf file." << endl;
    exit(EXIT_FAILURE);
  }

  deltax = prepop.sheetlength()/longside;

  *oldp[0] = vector<double>(nodes,p[0]);
  *oldp[1] = vector<double>(nodes,p[0]);
  *oldQ[0] = vector<double>(nodes,Q);
  *oldQ[1] = vector<double>(nodes,Q);

  dt2on12 = deltat*deltat/12.0;
  dfact = dt2on12*gamma*gamma;
  dt2ondx2 = (deltat*deltat)/(deltax*deltax);
  p2 = dt2ondx2*range*range*gamma*gamma;
  tenminus3p2 = 10.0 - 3.0*p2;
  twominus3p2 =  2.0 - 3.0*p2;
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

  if( gamma*range*deltat/deltax > 1.0/sqrt(2.0)) {
    cerr << "Wave equation with gamma: " << gamma << " axonal range: " << range << endl;
    cerr << "and deltat: " << deltat << " deltax: " << deltax << endl;
    cerr << "does not fulfill the Courant condition" << endl;
    cerr << "Courant number is: " << (gamma*range*deltat/deltax) << endl;
    cerr << "and should be less than " << 1.0/sqrt(2.0) << endl;
    exit(EXIT_FAILURE);
  }
}

Wave::Wave( size_type nodes, double deltat, size_type index, Population& prepop,
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

  for( size_type i=0; i<nodes; i++,
       stencilp++, stencilQ++, stencil_oldp++, stencil_oldQ++ ) {
    sump     = stencilp(stencilp.n)  +stencilp(stencilp.s)  +stencilp(stencilp.w)  +stencilp(stencilp.e) ; // sum of the von Neumann (orthogonal) neighbourhood (phi)
    diagsump = stencilp(stencilp.nw) +stencilp(stencilp.ne) +stencilp(stencilp.sw) +stencilp(stencilp.se); // sum of the diagonal neighbourhood (phi)
    sumQ     = stencilQ(stencilQ.n)  +stencilQ(stencilQ.s)  +stencilQ(stencilQ.w)  +stencilQ(stencilQ.e) ; // sum of the von Neumann (orthogonal) neighbourhood (Q)
    diagsumQ = stencilQ(stencilQ.nw) +stencilQ(stencilQ.ne) +stencilQ(stencilQ.sw) +stencilQ(stencilQ.se); // sum of the diagonal neighbourhood (Q)
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
