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

  double theta;
  double sigma;
  double Q_max;
  double gradient;
  double intercept;

  Array<Dendrite> dendrites; // array of dendrites
  vector<double> v; // soma potential for the population
protected:
  void init( Configf& inputf );
  void restart( Restartf& restartf );
  void dump( Dumpf& dumpf ) const;
public: 
  QResponse( int nodes, double deltat, int index );
  virtual ~QResponse(void);
  void step(void);
  void add2Dendrite( int index,
          const Propag& prepropag, const Couple& precouple );

  void fire( vector<double>& Q ) const;
  const vector<double>& V(void) const;
};

#endif
