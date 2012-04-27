#ifndef BURST_H
#define BURST_H

#include"qresponse.h"
#include"output.h"

class BurstResponse : public QResponse
{
  BurstResponse(BurstResponse& ); // no copy constructor
  BurstResponse(void);      // no copy constructor
protected:
  void init( Configf& inputf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;
public: 
  BurstResponse( int nodes, double deltat, int index );
  virtual ~BurstResponse(void);
  void step(void);

  void fire( vector<double>& Q ) const;
  const vector<double>& V(void) const;
};

#endif
