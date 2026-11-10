/**
 * @file wave.cpp
 * @brief Wave propagator definitions.
 *
 * As well as a constructor and destructor, this file defines the init and
 * step member functions. This Wave Propagator is based on the version described
 * by Rennie 2001. Specifically, Wave::step() implements Equation A.16 of that
 * PhD Thesis..
 * Maybe something more about discrete numerical approximations to Laplacian operators here...
 *
 * Rennie, Christopher; "Modeling the Large-scale Electrical Activity of the Brain."; PhD Thesis, 2001.
 *
 * @author Peter Drysdale, Felix Fung, Romesh Abeysuriya, Paula Sanz-Leon,
 *
 */

// Main module header
#include "wave.h"       // Wave;

// Other nftsim headers
#include "configf.h"    // Configf;
#include "make_unique.h" // nftsim::make_unique;
#include "parallel.h"   // NFTSIM_PARALLEL_FOR;
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

  // Both previous steps of phi start at the initial phi (node by node), so
  // phi starts with zero time derivative.
  *oldp[0] = p;
  *oldp[1] = p;
  *oldQ[0] = vector<double>(nodes,Q);
  *oldQ[1] = vector<double>(nodes,Q);

  dt2on12 = deltat*deltat/12.0;
  dfact = dt2on12*gamma*gamma;
  dt2ondx2 = (deltat*deltat)/(deltax*deltax);
  p2 = dt2ondx2*range*range*gamma*gamma;
  tenminus4p2 = 10.0 - 4.0*p2;
  twominus4p2 =  2.0 - 4.0*p2;
  expfactneg = exp(-deltat*gamma);
  expfactpos = exp(deltat*gamma);

  // The damping time 1/gamma must be resolved by the time-step and the axonal
  // range by the grid. (This used to test gamma/2 < deltat, which compares a
  // rate with a time and so could never fail.)
  if(deltat > 0.5/gamma || range/2.0 < deltax) {
    cerr << "Wave equation with gamma: " << gamma << " [s^-1] and range: " << range << " [m]" << endl;
    cerr << "is not adequately resolved by the chosen time-step and grid spacing." << endl;
    cerr << "gamma = " << gamma << " [s^-1] requires deltat <= " << 0.5/gamma << " [s]" << endl;
    cerr << "and range = " << range << " [m] requires deltax <= " << range/2.0 << " [m]" << endl;
    cerr << "Current deltat is = " << deltat << " [s] and deltax is = " << deltax << " [m]" << endl;
    exit(EXIT_FAILURE);
  }

  //Validate (0 < p <= 1/sqrt(2)), NOTE: there is a typo after A.13 in Rennie 2001, see, A. R. Mitchell.
  //Computational methods in partial differential equations. John Wiley & Sons, London, 1969.
  //The general formula for the maximum value of p is given in page 205 of the book reference above.
  if( sqrt(p2) > 1.0/sqrt(2.0)) {
    cerr << "Wave equation with gamma: " << gamma << " axonal range: " << range << endl;
    cerr << "and deltat: " << deltat << " deltax: " << deltax << endl;
    cerr << "does not fulfill the Courant condition" << endl;
    cerr << "Courant number is: " << sqrt(p2) << endl;
    cerr << "and should be less than " << 1.0/sqrt(2.0) << endl;
    cerr << "You need larger deltax or smaller deltat." << endl;
    exit(EXIT_FAILURE);
  }
}

Wave::Wave( size_type nodes, double deltat, size_type index, Population& prepop,
            Population& postpop, size_type longside, string topology )
  : Propagator(nodes,deltat,index,prepop,postpop,longside,topology),
    key(0) {
  for( int k=0; k<2; k++ ) {
    oldp[k] = nftsim::make_unique<Stencil>(nodes,longside,topology);
    oldQ[k] = nftsim::make_unique<Stencil>(nodes,longside,topology);
  }
}

Wave::~Wave() = default;

/**
  @brief Computes the updated \f$\phi\f$ using the Wave Propagator function.

  The Wave propagator equation is a five-point stencil based method given by
  Equation A.16 of Rennie 2001:
  \f[
    u_{l,m}^{n+1} = (2 - 4p^2)u_{l,m}^n +
                    p^2(u_{l+1,m}^n + u_{l-1,m}^n + u_{l,m+1}^n + u_{l,m-1}^n) -
                    u_{l,m}^{n-1} +
                    \frac{k^2\gamma^2}{12}\left[(10 - 4p^2)w_{l,m}^n + (w_{l,m}^{n+1} + w_{l,m}^{n-1}) +
                                                  p^2(w_{l+1,m}^n + w_{l-1,m}^n + w_{l,m+1}^n + w_{l,m-1}^n)\right]
  \f]
  where superscripts index time, with current time \f$n\f$; subscripts index
  space, with current location \f$l,m\f$; also
  \f{eqnarray*}{
    u({\mathbf r},t)   &=& \phi({\mathbf r},t) e^{\gamma t} \\
    w({\mathbf r},t)   &=& Q({\mathbf r},t) e^{\gamma t}    \\
    p^2 &=& \left(\frac{kr\gamma}{h}\right)^2
  \f}
  where, considering \f$n=0\f$; \f$n+1={\Delta}t\f$; and \f$n-1=-{\Delta}t\f$, we use:
  \f{eqnarray*}{
    u^{n+1} &=& \phi^{n+1}\exp(\gamma{\Delta}t)  \\
    u^n     &=& \phi^n                           \\
    u^{n-1} &=& \phi^{n-1}\exp(-\gamma{\Delta}t) \\
    w^{n+1} &=& Q^{n+1}\exp(\gamma{\Delta}t)     \\
    w^n     &=& Q^n                              \\
    w^{n-1} &=& Q^{n-1}\exp(-\gamma{\Delta}t)
  \f}
  giving
  \f{eqnarray*}{
    \phi_{l,m}^{n+1} &=& \exp(-\gamma{\Delta}t) \left((2 - 4p^2)u_{l,m}^n +
                         p^2(u_{l+1,m}^n + u_{l-1,m}^n + u_{l,m+1}^n + u_{l,m-1}^n) -
                         u_{l,m}^{n-1} +
                         \frac{k^2\gamma^2}{12}\left[(10 - 4p^2)w_{l,m}^n + (w_{l,m}^{n+1} + w_{l,m}^{n-1}) +
                                                  p^2(w_{l+1,m}^n + w_{l-1,m}^n + w_{l,m+1}^n + w_{l,m-1}^n)\right]\right) \\
                     &=& \exp(-\gamma{\Delta}t) \left((2 - 4p^2)\phi_{l,m}^n +
                         p^2(\phi_{l+1,m}^n + \phi_{l-1,m}^n + \phi_{l,m+1}^n + \phi_{l,m-1}^n) -
                         \phi_{l,m}^{n-1}\exp(-\gamma{\Delta}t) +
                         \frac{k^2\gamma^2}{12}\left[(10 - 4p^2)Q_{l,m}^n + Q_{l,m}^{n+1}\exp(\gamma{\Delta}t) + Q_{l,m}^{n-1}\exp(-\gamma{\Delta}t) +
                                                  p^2(Q_{l+1,m}^n + Q_{l-1,m}^n + Q_{l,m+1}^n + Q_{l,m-1}^n)\right]\right)
  \f}
  and as \f$k={\Delta}t\f$ and \f$h={\Delta}x\f$:
  \f[
    p^2 = \frac{{\Delta}t^2}{{\Delta}x^2}r^2\gamma^2
  \f]
  with the requirement:
  \f[
    0 \lt p \leq \frac{1}{\sqrt{2}}.
  \f]

*/
void Wave::step() {
  //Most recent phi and Q are at key; key == 0 evaluates to second most recent phi and Q.
  Stencil& stencilp = *oldp[key];
  Stencil& stencil_oldp = *oldp[key == 0];
  Stencil& stencilQ = *oldQ[key];
  Stencil& stencil_oldQ = *oldQ[key == 0];

  // Delayed Q at the new time-step. Fetched once: it used to be re-evaluated
  // for every node, which for inhomogeneous Tau rebuilt the whole delayed
  // vector each time (O(nodes^2) per step).
  const vector<double>& Qnew = prepop.Q(tau);

  //Iterate over space, calculating phi for the next time-step. The four
  //stencils have the same layout, so node i is at index k = padded(i) of each,
  //with neighbours n = k-stride, s = k+stride, w = k-1 and e = k+1.
  const double* up = stencilp.data();
  const double* uq = stencilQ.data();
  const double* oldup = stencil_oldp.data();
  const double* olduq = stencil_oldQ.data();
  const int stride = stencilp.stride();
  NFTSIM_PARALLEL_FOR
  for( size_type i=0; i<nodes; i++ ) {
    const int k = stencilp.padded(static_cast<int>(i));
    const double sump  = up[k-stride] + up[k+stride] + up[k-1] + up[k+1] ; // sum of the von Neumann (orthogonal) neighbourhood (phi)
    const double sumQ  = uq[k-stride] + uq[k+stride] + uq[k-1] + uq[k+1] ; // sum of the von Neumann (orthogonal) neighbourhood (Q)
    const double drive = dfact * (tenminus4p2*uq[k] +
                     Qnew[i]*expfactpos +
                     olduq[k]*expfactneg +
                     p2*sumQ);
    p[i]  = expfactneg * (twominus4p2*up[k] +
                          p2*sump -
                          oldup[k]*expfactneg +
                          drive);
  }

  //If key==1, set it to 0; if key==0, set it to 1.
  key = static_cast<int>(key == 0);
  stencil_oldp = p;
  stencil_oldQ = Qnew;
}
