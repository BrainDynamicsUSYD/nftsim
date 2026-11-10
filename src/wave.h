/**
 * @file wave.h
 * @brief Wave propagator declaration.
 *
 * The wave propagator declaration, including parameters from the wave equation
 * and coefficients related to the five-point stencil implementation.
 *
 * Wave (`Propagator N: Wave`) solves the damped wave equation on the grid
 * with the scheme of Rennie (2001), Eq. A.16, using Stencil. WaveLegacy
 * (wave_legacy.h, `Propagator N: WaveLegacy`) is an older nine-point version,
 * kept to reproduce published results. They share no code beyond the
 * Propagator base and the StencilGrid behind their stencils, because their
 * coefficients and update formulas differ. With one node, both are replaced
 * by Harmonic (see Solver::init).
 *
 * @author Peter Drysdale, Felix Fung, Romesh Abeysuriya, Paula Sanz-Leon,
 *
 */

#ifndef NFTSIM_SRC_WAVE_H
#define NFTSIM_SRC_WAVE_H

// Other nftsim headers
#include "configf.h"    // Configf;
#include "population.h" // Population;
#include "propagator.h" // Propagator;
#include "stencil.h"    // Stencil;

// C++ standard library headers
#include <memory>  // std::unique_ptr;
#include <string>  // std::string

class Wave : public Propagator {
 protected:
  void init( Configf& configf ) override;

  std::unique_ptr<Stencil> oldp[2]; ///< keyring stencil to past phi, oldp[key]==most recent
  std::unique_ptr<Stencil> oldQ[2]; ///< keyring stencil to past Q, oldQ[key]==most recent
  int key;          ///< used as an index into a length 2 cyclical history of phi and Q.

  // variables that are initialized once only
  double deltax;      ///< Grid spacing in space - spatial resolution
  double dt2on12;
  double dfact;
  double dt2ondx2;    ///< Factor in wave equation equal to ((gamma deltat)^2)/12.
  double p2;          ///< Dimensionless step-size squared. Square of the mesh ratio.
  double tenminus4p2; ///< Factor in wave algorithm
  double twominus4p2; ///< Factor in wave algorithm
  double expfactneg;  ///< Exp(- gamma deltat) - invert substitutions \f$u={\phi}e^{\gamma t}\f$; \f$w=Q e^{\gamma t}\f$.
  double expfactpos;  ///< Exp(gamma deltat) - invert substitutions \f$u={\phi}e^{\gamma t}\f$; \f$w=Q e^{\gamma t}\f$.

 public:
  Wave() = delete;            // No default constructor allowed.
  Wave(const Wave&) = delete; // No copy constructor allowed.

  Wave( size_type nodes, double deltat, size_type index, Population& prepop,
        Population& postpop, size_type longside, std::string topology );
  ~Wave() override;
  void step() override;
};

#endif //NFTSIM_SRC_WAVE_H
