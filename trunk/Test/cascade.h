#ifndef CASCADE_H
#define CASCADE_H

#include"cadp.h"
#include<vector>
using std::vector;

// abstract model of protein cascade
class Cascade : public CaDP
{
  Cascade();
  Cascade(Cascade&);
protected:
  Cascade* nextcascade;
  struct CascadeDE : public CaDE
  {
    bool temporary;
    CascadeDE( int nodes, double deltat, bool temporary )
        : CaDE(nodes,deltat), temporary(temporary) {}
    virtual ~CascadeDE(void) {}
    virtual void init( Configf& configf );
    virtual void rhs( const vector<double>& y, vector<double>& dydt );
    virtual double _x( double Ca ) const;
    virtual double _y( double Ca ) const;
  };
public:
  Cascade( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop, Cascade* nextcascade );
  virtual ~Cascade(void);
  virtual void input( vector<double>& input, double max );
  virtual void initnu( double initnu );
  virtual void step(void);
  virtual void output( Output& output ) const;
  virtual const vector<double>& nu(void) const;
};

class CaCascade: public CaDP
{
  CaCascade();
  CaCascade(Cascade&);
protected:
  vector<Cascade*> cascade;
  struct CaCascadeDE : public CaDE
  {
    CaCascadeDE( int nodes, double deltat ) : CaDE(nodes,deltat) {}
    virtual ~CaCascadeDE(void) {}
    virtual void rhs( const vector<double>& y, vector<double>& dydt );
  };
  virtual void init( Configf& configf );
public:
  CaCascade( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop );
  virtual ~CaCascade(void);
  virtual void step(void);
  virtual void output( Output& output ) const;
  virtual const vector<double>& nu(void) const;
};

#endif
