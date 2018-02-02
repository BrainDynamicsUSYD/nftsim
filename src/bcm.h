/** @file bcm.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_BCM_H
#define NFTSIM_SRC_BCM_H

// Other nftsim headers
#include "cadp.h"       // CaDP;
#include "configf.h"    // Configf;
#include "output.h"     // Output;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <vector> // std::vector;

class BCM : public CaDP {
 protected:
  //double t_BCM;
  struct BCMDE : public CaDE {
    double t_BCM = 0.0;
    double t_rec = 0.0;
    double gnmda_0 = 0.0;
    BCMDE( size_type nodes, double deltat ) : CaDE(nodes,deltat) {
      extend(1);
    }
    BCMDE( BCMDE& bcmDE ) : CaDE(bcmDE.nodes,bcmDE.deltat) {
      *this = bcmDE;
    }
    BCMDE& operator= ( const BCMDE& bcmDE ) {
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
        for( size_type j=0; j<nodes; j++ ) {
          this->variables[i][j] = bcmDE.variables[i][j];
        }
      }
      return *this;
    }
    ~BCMDE() override = default;
    void init( Configf& configf ) override;
    void rhs( const std::vector<double>& y, std::vector<double>& dydt, size_type /*unused*/ ) override;
  };
 public:
  BCM(const BCM&) = delete; // No copy constructor allowed.
  BCM() = delete;           // No default constructor allowed.
  BCM( size_type nodes, double deltat, size_type index,
       const Propagator& prepropag, const Population& postpop );
  ~BCM() override;
  void output( Output& output ) const override;
};

#endif //NFTSIM_SRC_BCM_H
