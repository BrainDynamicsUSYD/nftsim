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
  vector< vector<double> > qhistory; // keyring of Q
  int qkey; // index to the present q in qhistory
  QResponse* qresponse; // qresponse for neural population
  Timeseries* timeseries; // timeseries for stimulus
  bool settled; // if true, forbids add2Dendrite and growHistory
protected:
  void init( Configf& inputf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;
public:
  Population( int nodes, double deltat, int index );
  virtual ~Population();
  void step(void);
  const vector<double>& Q( const Tau& tau = Tau() ) const;
  const vector<double>& operator()( const Tau& tau = Tau() ) const;
  double Qinit( Configf& configf ) const;
  const vector<double>& V(void) const;
  double operator[]( int node ) const;
  void add2Dendrite( int index,
          const Propag& prepropag, const Couple& precouple, Configf& configf );
  void growHistory( const Tau& tau );
  virtual vector<Output*> output(void) const;
};

#endif
