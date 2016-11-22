/** @file bcmlong.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_BCMLONG_H
#define NEUROFIELD_SRC_BCMLONG_H

#include "bcm.h"

class BCMLong : public BCM {
  BCMLong();
  BCMLong(BCMLong&);
 protected:
  void init( Configf& configf ) override;

  vector<BCMDE*> de_2d;
  vector<RK4*> rk4_2d;

 public:
  BCMLong( size_type nodes, double deltat, size_type index,
           const Propagator& prepropag, const Population& postpop );
  ~BCMLong(void) override;
  void step(void) override;
  void output( Output& output ) const override;
};

#endif //NEUROFIELD_SRC_BCMLONG_H
