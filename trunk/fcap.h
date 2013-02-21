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
    /*class Response : public DE //public NF
    {
      vector<double>& input;
      double lambda, mu;
      vector<double> value;
    public:
      void init( Configf& configf );
      Response( int nodes, double deltat, vector<double>& input )
          : DE(nodes,deltat,3), input(input), value(nodes) {}
      virtual ~Response(void) {}
      virtual void rhs( const vector<double>& y, vector<double>& dydt );
    };
    Response xresponse; RK4 xrk4;
    Response yresponse; RK4 yrk4;*/

    class Response
    {
      double alpha, deltat;
      double lambda, mu;
      FractionalIntegral d1, d2;
      double _d1, _d2;
    public:
      void init( Configf& configf );
      Response( double alpha, double deltat, double init )
          : alpha(alpha), deltat(deltat),
          d1(alpha,deltat), d2(alpha,deltat), _d1(init) {}
      ~Response(void) {}
      operator double() const;
      void input( double input );
    };
    Response xresponse; Response yresponse;

    virtual void init( Configf& configf );
    fCaDE( int nodes, double deltat )
        : CaDE(nodes,deltat),
        xresponse(0.7,deltat,1e-4), yresponse(0.7,deltat,1e-4)
          /*xresponse(nodes,deltat,variables[4]), xrk4(xresponse),
          yresponse(nodes,deltat,variables[5]), yrk4(yresponse)*/ {}
    virtual ~fCaDE(void) {}
    void pot(void);
    void dep(void);
  };

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
