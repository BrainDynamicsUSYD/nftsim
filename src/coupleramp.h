/**
 * @file coupleramp.h
 * Couple class to produce piecewise linear segments to vary synaptic couplings nu.
 * In other words it produces a temporal profile for nu.
 * 
 * @brief Can use "brief" tag to explicitly generate comments for file documentation.
 *
 * @author Paula Sanz-Leon , Romesh Abeysuriya
 * 
 */
  
/**
   Define properties of CoupleRamp
  
   + param[in]     ns, tpts: vector with the values of nus at specific time points specified in vector tpts.
   + param[in]     pairs   : total number of pairs of (nu, time) to define the segments
*/

#ifndef COUPLERAMP_H
#define COUPLERAMP_H

#include"couple.h"

using std::vector;

class CoupleRamp : public Couple
{
  CoupleRamp();
  CoupleRamp(CoupleRamp&);
protected:
  vector<double> ns;
  vector<double> tpts;
  vector<double> deltanu;
  double time;
  int pairs; 
public:
  void init( Configf& configf );
  void step(void);
  CoupleRamp( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop );
  virtual ~CoupleRamp(void);
};

#endif