#ifndef FCAP_H
#define FCAP_H

#include<deque>
using std::deque;
#include"cadp.h"

class fCaP : public CaDP
{
  fCaP();
  fCaP(fCaP&);
protected:
  struct FractionalIntegral
  {
    operator double() const;
    void newHistory( double newest_history );
    FractionalIntegral( double alpha, double deltat )
        : alpha(alpha), deltat(deltat) {}
  private:
    double alpha;
    double deltat;
    deque<double> history;
  };

  struct fCaDE : public CaDE
  {
    double alpha, beta;
    double lambda_x, mu_x;
    double lambda_y, mu_y;
    vector<FractionalIntegral*> x1;
    vector<FractionalIntegral*> x2;
    vector<FractionalIntegral*> y1;
    vector<FractionalIntegral*> y2;
    virtual void init( Configf& configf );
    void rhs( const vector<double>& y, vector<double>& dydt );
    fCaDE( int nodes, double deltat )
        : CaDE(nodes,deltat), x1(nodes), x2(nodes), y1(nodes), y2(nodes) {}
    virtual ~fCaDE(void) {
      for( int i=0; i<nodes; i++ ) {
        delete x1[i];
        delete x2[i];
        delete y1[i];
        delete y2[i];
      }
    }
    void pot(void);
    void dep(void);
  };

  /*double alpha;
  vector< deque<double> > history1st; // == eta*( old dnudt in CaDP )
  vector< deque<double> > history2nd; // == D^-alpha(history1st) -eta*nu
  vector<double> oldnu; // to calculate old dnudt*/
  double lambda, mu;
  vector<FractionalIntegral*> newnu;
  vector<FractionalIntegral*> newnu2;
  vector<double> oldnu;
  double zeta;
  double init_nu; // initial condition

  virtual void init( Configf& configf );
  virtual void restart( Restartf& restartf );
  virtual void dump( Dumpf& dumpf ) const;
public: 
  fCaP( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop );
  virtual ~fCaP(void);
  virtual void step(void);
};

#endif
