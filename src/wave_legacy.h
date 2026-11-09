/**
 * @file wave_legacy.h
 * @brief WaveLegacy propagator declaration.
 *
 * The WaveLegacy propagator declaration, including parameters from the wave
 * equation and coefficients related to the nine-point stencil implementation.
 *
 * WaveLegacy (`Propagator N: WaveLegacy`) is the earlier nine-point form of
 * Wave (wave.h). It was found to be incorrectly derived (see
 * wave_legacy.cpp) and is kept only to reproduce published results; use Wave
 * for new work.
 *
 *
 * @author Peter Drysdale, Felix Fung, Romesh Abeysuriya, Paula Sanz-Leon.
 *
 */

#ifndef NFTSIM_SRC_WAVE_LEGACY_H
#define NFTSIM_SRC_WAVE_LEGACY_H

// Other nftsim headers
#include "configf.h"         // Configf;
#include "population.h"      // Population;
#include "propagator.h"      // Propagator;
#include "stencil_legacy.h"  // StencilLegacy;

// C++ standard library headers
#include <memory>  // std::unique_ptr;
#include <string>  // std::string

class WaveLegacy : public Propagator {
  WaveLegacy(); // no default constructor
  WaveLegacy(WaveLegacy&); // no copy constructor
 protected:
  void init( Configf& configf ) override;

  std::unique_ptr<StencilLegacy> oldp[2]; ///< keyring stencil to past phi, oldp[key]==most recent
  std::unique_ptr<StencilLegacy> oldQ[2]; ///< keyring stencil to past Q, oldQ[key]==most recent
  int key;

  // variables that's initialized once only
  double deltax; ///< Grid spacing in space - spatial resolution
  double dt2on12;
  double dfact;
  double dt2ondx2;    ///< Factor in wave equation equal to ((gamma deltat)^2)/12.
  double p2;          ///< Fquare of the mesh ratio
  double tenminus3p2; ///< Factor in wave algorithm
  double twominus3p2; ///< Factor in wave algorithm
  double expfact1;    ///< Factor of Exp(- gamma deltat) - scale factor applied to each step due to damping
  double expfact2;    ///< Factor of Exp(- 2 gamma deltat)
 public:
  WaveLegacy( size_type nodes, double deltat, size_type index, Population& prepop,
        Population& postpop, size_type longside, std::string topology );
  ~WaveLegacy() override;
  void step() override;
};

#endif //NFTSIM_SRC_WAVE_LEGACY_H
