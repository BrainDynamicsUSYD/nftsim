/** @file bcm.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_BCM_H
#define NEUROFIELD_SRC_BCM_H

#include"cadp.h"

class BCM : public CaDP {
  BCM(BCM&);
  BCM();
 protected:
  //double t_BCM;
  struct BCMDE : public CaDE {
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
      for( size_t i=0; i<variables.size(); i++ ) {
        for( int j=0; j<nodes; j++ ) {
          this->variables[i][j] = bcmDE.variables[i][j];
        }
      }
      return *this;
    }
    ~BCMDE(void) override = default;
    void init( Configf& configf ) override;
    void rhs( const vector<double>& y, vector<double>& dydt ) override;
  };
 public:
  BCM( int nodes, double deltat, int index,
       const Propagator& prepropag, const Population& postpop );
  ~BCM(void) override;
  void output( Output& output ) const override;
};

#endif
