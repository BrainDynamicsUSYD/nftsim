#ifndef QRESPONSE_H
#define QRESPONSE_H

#include"array.h"
#include"dendrite.h"
#include"configf.h"
#include"nf.h"

class Dendrite;
class Propag;
class Couple;

class QResponse : public NF
{
  QResponse(QResponse& ); // no copy constructor
  QResponse(void);      // no copy constructor

protected:
  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;

  double theta;
  double sigma;
  double Q_max;
  double gradient;
  double intercept;
  
  Array<Dendrite> dendrites; // array of dendrites
  vector<double> v; // soma potential for the population
public: 
  QResponse( int nodes, double deltat, int index );
  virtual ~QResponse(void);
  virtual void step(void);
  virtual void add2Dendrite( int index,
          const Propag& prepropag, const Couple& precouple );

  virtual void fire( vector<double>& Q ) const;
  virtual const vector<double>& V(void) const;
  virtual vector<Output*> output(void) const;
};

#endif
