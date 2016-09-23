/**
 * @file wave.h
 * @brief Wave propagator definition
 *
 * The wave propagator definition, inlcuding parameters from the wave equation
 * and coefficients related to the nine-point stencil implementation.
 *
 *
 * @author Peter Drysdale, Felix Fung, Romesh Abeysuriya, Paula Sanz-Leon
 *
 */

/**
   Reads from the configuration file

*/
#ifndef NEUROFIELD_SRC_WAVE_H
#define NEUROFIELD_SRC_WAVE_H

#include"propag.h"
#include"stencil.h"

class Wave : public Propag {
  Wave(); // no default constructor
  Wave(Wave&); // no copy constructor
 protected:
  void init( Configf& configf ) override;
  //void restart( Restartf& restartf );
  //void dump( Dumpf& dumpf ) const;

  Stencil* oldp[2]; // keyring stencil to past phi, oldp[key]==most recent
  Stencil* oldQ[2]; // keyring stencil to past Q, oldQ[key]==most recent
  int key;

  // variables that's initialized once only
  double deltax; // Grid spacing in space - spatial resolution
  double range;  // Characteristic axonal range
  double gamma;  // Damping coefficient
  double dt2on12;
  double dfact;
  double dt2ondx2;    // Factor in wave equation equal to ((gamma deltat)^2)/12.
  double p2;          // Fquare of the mesh ratio
  double tenminus3p2; // Factor in wave algorithm
  double twominus3p2; // Factor in wave algorithm
  double expfact1;    // Factor of Exp(- gamma deltat) - scale factor applied to each step due to damping
  double expfact2;    // Factor of Exp(- 2 gamma deltat)

  // variables that are changed every timestep
  double sump;     // sum of the points in the von Neumann neighbourhood (phi)
  double diagsump; // sum of the points in the diagonal neighbourhood (phi)
  double sumQ;     // sum of the points in the von Neumann neighbourhood (Q)
  double diagsumQ; // sum of the points in the von Neumann neighbourhood (Q)
  double drive;
 public:
  Wave( int nodes, double deltat, int index, Population& prepop,
        Population& postpop, int longside, string topology );
  ~Wave(void) override;
  void step(void) override;
};

#endif
