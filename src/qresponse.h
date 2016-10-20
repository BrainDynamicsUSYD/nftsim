/** @file qresponse.h
  @brief Defines the QResponse class, for the soma response of neural populations.

  Each neural population is associated with a QResponse object, which produces
  the soma response.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_QRESPONSE_H
#define NEUROFIELD_SRC_QRESPONSE_H

#include"array.h"
#include"de.h"
#include"population.h"

using std::string;

class Dendrite;
class Propagator;
class Couple;

class QResponse : public NF {
  QResponse(QResponse& ); // no copy constructor
  QResponse(void);      // no copy constructor

 protected:
  void init( Configf& configf ) override;
  //virtual void restart( Restartf& restartf );
  //virtual void dump( Dumpf& dumpf ) const;

  string mode;
  double theta;
  double sigma;
  double Q_max; ///< Maximum firing rate.
  double a,b,c,d;

  Array<Dendrite> dendrites; ///< array of dendrites
  vector<int> dendrite_index; ///< indices of dendrites
  vector<double> v; ///< soma potential for the population

  // glutamate concentration in synaptic cleft
  struct Glu : public DE {
    double Lambda; ///< glutamate concentration per action potential
    double tGlu;   ///< time scale of glutamate

    Glu( int nodes, double deltat ) : DE(nodes,deltat,2) {}
    ~Glu(void) override = default;
    void init( Configf& configf );
    void rhs( const vector<double>& y, vector<double>& dydt ) override;
  };
  Glu glu_m;
  RK4 glu_rk4;
 public:
  QResponse( int nodes, double deltat, int index );
  ~QResponse(void) override;
  void step(void) override;
  virtual void add2Dendrite( int index,
                             const Propagator& prepropag, const Couple& precouple, Configf& configf );
  const vector<double>& glu(void) const;

  virtual void fire( vector<double>& Q ) const;
  inline const vector<double>& V(void) const;
  void output( Output& output ) const override;
  virtual void outputDendrite( int index, Output& output ) const;
};

const vector<double>& QResponse::V(void) const {
  return v;
}

#endif
