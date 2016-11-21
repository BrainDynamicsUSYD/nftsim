/** @file coupling.h
  @brief A new Coupling class, where nu_ab follows a smooth function.

  The smooth variation of nu_ab is defined by the difference of
  two arctangent functions.


  @author Farah Deeba, Paula Sanz-Leon, Sahand Assadzadeh
*/

#ifndef COUPLING_DIFF_ARCTAN_H
#define COUPLING_DIFF_ARCTAN_H

#include "coupling.h"
using std::vector;

class CouplingDiffArctan : public Coupling {
  CouplingDiffArctan();
  CouplingDiffArctan(CouplingDiffArctan&);
 protected:
  double nu_min, nu_max;
  double t_half_down, t_half_up;
  double delta; ///< Time interval [s] in which the function will go from 0.25 to 0.75 of nu_max.
  double time_tot;
  double diff_atan, diff_atan_min,  diff_atan_max;
  double time;
  double time_int;
  vector<double> deltanu;

 public:
  void init( Configf& configf );
  void step(void);
  void find(void);
  CouplingDiffArctan( size_type nodes, double deltat, size_type index,
                    const Propagator& prepropag, const Population& postpop, double tempf );
  virtual ~CouplingDiffArctan(void);
};

#endif //COUPLING_DIFF_ARCTAN_H

