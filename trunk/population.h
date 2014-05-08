#ifndef POPULATION_H
#define POPULATION_H

class Propag;
class Couple;
class Population;
class QResponse;

#include"timeseries.h"
#include"qresponse.h"
#include"couple.h"
#include"tau.h" // Must be included before propag.h
#include"propag.h"


using std::vector;


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
  double length; // spatial length
  double qinit; // initial firing rate

  virtual void init( Configf& configf );
public:
  Population( int nodes, double deltat, int index );
  virtual ~Population();
  virtual void step(void);
  virtual const vector<double>& Q( const Tau& tau ) const;
  double Qinit( Configf& configf ) const;
  virtual const vector<double>& V(void) const;
  inline double operator[]( int node ) const;
  const vector<double>& glu(void) const;
  inline double sheetlength(void) const { return length; }
  virtual void add2Dendrite( int index,
          const Propag& prepropag, const Couple& precouple, Configf& configf );
  virtual void growHistory( const Tau& tau );
  virtual void output( Output& output ) const;
  virtual void outputDendrite( int index, Output& output ) const;
};

#endif
