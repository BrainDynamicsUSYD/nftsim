#ifndef CA2_H
#define CA2_H

#include"cadp.h"

class Ca2 : public CaDP
{
  Ca2(Ca2&);
  Ca2();
protected:
  struct Ca2DE : public CaDE
  {
    Ca2DE( int nodes, double deltat ) : CaDE(nodes,deltat) {
      extend(1);
    }
    virtual ~Ca2DE(void) {}
    virtual void rhs( const vector<double>& y, vector<double>& dydt );
  };
public:
  Ca2( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop )
      : CaDP(nodes,deltat,index,glu,prepropag,postpop) {
    delete de; delete rk4;
    de = new Ca2DE(nodes,deltat); rk4 = new RK4(*de);
  }
  virtual ~Ca2(void) {}
  virtual void output( Output& output ) const {
    CaDP::output(output);
    output("dndt",(*de)[6]);
  }
};

#endif
