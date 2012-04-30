#ifndef BURST_H
#define BURST_H

#include"qresponse.h"
#include"output.h"

class BurstResponse : public QResponse //derived class; constructor initializer
{
  BurstResponse(BurstResponse& ); // no copy constructor; no explicit var name
  BurstResponse(void);      // no copy constructor; void no return value
protected:
  void init( Configf& inputf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const; //const member fnctn does not change obj
public: 
  BurstResponse( int nodes, double deltat, int index );
  virtual ~BurstResponse(void); //destructor mem fnctn 
								//virtual= needs redef in derived classes
  void step(void);

  void fire( vector<double>& Q ) const; //const refers to object called on
  const vector<double>& V(void) const;
  vector<Output*> output(void) const; //vector of Output ptrs filled by
};

#endif
