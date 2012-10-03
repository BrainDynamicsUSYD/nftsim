#ifndef POPULATION_H
#define POPULATION_H

#include"array.h"
using std::vector;

#include"output.h"
#include"tau.h"
#include"qresponse.h"
//#include"burst.h"
#include"timeseries.h"
#include"propag.h"
#include"couple.h"
#include"configf.h"
#include"nf.h"

class QResponse;
class Propag;

class Population : public NF
{
  Population(void); // no default constructor
  Population(Population& ); // no copy constructor
  QResponse* qresponse; // qresponse for neural population
  Timeseries* timeseries; // timeseries for stimulus
protected:
  int qkey; // index to the present q in qhistory
  vector< vector<double> > qhistory; // keyring of Q
  bool settled; // if true, forbids add2Dendrite and growHistory
  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public:
  Population( int nodes, double deltat, int index );
  virtual ~Population();
  virtual void step(void);
  virtual const vector<double>& Q( const Tau& tau = Tau() ) const;
  const vector<double>& operator()( const Tau& tau = Tau() ) const;
  double Qinit( Configf& configf ) const;
  const vector<double>& V(void) const;
  double operator[]( int node ) const;
  virtual void add2Dendrite( int index,
          const Propag& prepropag, const Couple& precouple, Configf& configf );
  virtual void growHistory( const Tau& tau );
  virtual vector<Output*> output(int req_index) const;
};

#endif
