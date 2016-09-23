#ifndef NEUROFIELD_SRC_BCMLONG_H
#define NEUROFIELD_SRC_BCMLONG_H

#include"bcm.h"

class BCMLong : public BCM {
  BCMLong();
  BCMLong(BCMLong&);
 protected:
  void init( Configf& configf ) override;

  vector<BCMDE*> de_2d;
  vector<RK4*> rk4_2d;

 public:
  BCMLong( int nodes, double deltat, int index,
           const Propag& prepropag, const Population& postpop );
  ~BCMLong(void) override;
  void step(void) override;
  void output( Output& output ) const override;
};

#endif
