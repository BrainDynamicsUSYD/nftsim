#ifndef QRESPONSE_H
#define QRESPONSE_H

#include<string>
using std::string;
#include"array.h"
#include"dendrite.h"
#include"configf.h"
#include"nf.h"
#include"de.h"

class Dendrite;
class Propag;
class Couple;

class QResponse : public NF
{
  QResponse(QResponse& ); // no copy constructor
  QResponse(void);      // no copy constructor

protected:
  virtual void init( Configf& configf );
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  string mode;
  double theta;
  double sigma;
  double Q_max;
  double a,b,c,d;

  Array<Dendrite> dendrites; // array of dendrites
  vector<int> dendrite_index; // indices of dendrites
  vector<double> v; // soma potential for the population

  // glutamate concentration in synaptic cleft
  struct Glu : public DE
  {
    double Lambda; // glutamate concentration per action potential
    double tGlu;   // time scale of glutamate

    Glu( int nodes, double deltat ) : DE(nodes,deltat,2) {}
    virtual ~Glu(void) {}
    void init( Configf& configf );
    void rhs( const vector<double>& y, vector<double>& dydt );
  };
  Glu glu_m;
  RK4 glu_rk4;
public: 
  QResponse( int nodes, double deltat, int index );
  virtual ~QResponse(void);
  virtual void step(void);
  virtual void add2Dendrite( int index,
          const Propag& prepropag, const Couple& precouple, Configf& configf );
  const vector<double>& glu(void) const;

  virtual void fire( vector<double>& Q ) const;
  inline const vector<double>& V(void) const;
  virtual void output( Output& output ) const;
  virtual void outputDendrite( int index, Output& output ) const;
};

const vector<double>& QResponse::V(void) const
{
  return v;
}

#endif
