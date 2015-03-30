#ifndef BCM_H
#define BCM_H

#include"cadp.h"

class BCM : public CaDP
{
  BCM(BCM&);
  BCM();
protected:
  //double t_BCM;
  struct BCMDE : public CaDE
  {
    double t_BCM;
    double t_rec;
    double gnmda_0;
    BCMDE( int nodes, double deltat ) : CaDE(nodes,deltat) {
      extend(1);
    }
    BCMDE( BCMDE& bcmDE ) : CaDE(bcmDE.nodes,bcmDE.deltat) {
      *this = bcmDE;
    }
    BCMDE& operator= ( BCMDE& bcmDE ) {
      this->nu_init = bcmDE.nu_init;
      this->B = bcmDE.B;
      this->glu_0 = bcmDE.glu_0;
      this->max = bcmDE.max;
      this->xth = bcmDE.xth;
      this->yth = bcmDE.yth;
      this->ltd = bcmDE.ltd;
      this->ltp = bcmDE.ltp;
      this->dth = bcmDE.dth;
      this->pth = bcmDE.pth;
      this->tCa = bcmDE.tCa;
      this->gnmda = bcmDE.gnmda;
      this->z = bcmDE.z;
      this->pos = bcmDE.pos;
      this->t_BCM = bcmDE.t_BCM;
      this->t_rec = bcmDE.t_rec;
      this->gnmda_0 = bcmDE.gnmda_0;
	  for( size_t i=0; i<variables.size(); i++ )
        for( int j=0; j<nodes; j++ )
          this->variables[i][j] = bcmDE.variables[i][j];
      return *this;
    }
    virtual ~BCMDE(void) {}
    virtual void init( Configf& configf );
    virtual void rhs( const vector<double>& y, vector<double>& dydt );
  };
public:
  BCM( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop );
  virtual ~BCM(void);
  virtual void output( Output& output ) const;
};

#endif
