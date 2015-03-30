#ifndef BCM_LONG_H
#define BCM_LONG_H

#include"bcm.h"

class BCMLong : public BCM
{
  BCMLong();
  BCMLong(BCMLong&);
protected:
  virtual void init( Configf& configf );

  vector<BCMDE*> de_2d;
  vector<RK4*> rk4_2d;

public: 
  BCMLong( int nodes, double deltat, int index,
      const Propag& prepropag, const Population& postpop );
  virtual ~BCMLong(void);
  virtual void step(void);
  virtual void output( Output& output ) const;
};

#endif
