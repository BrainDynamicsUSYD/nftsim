/**
 * @file coupleatan.cpp
 * Produce atan shaped temporal variation of nu.
 * Assumes that at t=0, nu=nus[0], ie, the segment between t=0 and timepoints[0] is constant.
 *
 * @author Farah Deeba, Paula Sanz-Leon
 * 
 */
  
/**
   Reads from the configuration file
 * + param[in]   nu0: Initial as well as equilibrium value of nu
 * + param[in]   nu_max: Corresponding value of nu at t1
 * + param[in]   delt : Width of the time ramp
 * + param[in]   t: Total time. 
 * + param[in]   t1: time at which time reaches half of its total range
 * + param[in]   t2: time at which time comes down half of its total range    
**/



#include<iostream>
#include <vector>
#include <algorithm>

using namespace std;
using std::cerr;
using std::endl;
using std::cout;

#include "coupleatan.h"


void CoupleAtan::init( Configf& configf )
{
  //read value of initial nu
  configf.param("nu0",nu0);
  //read value of maximum nu
  configf.param("nu_max",nu_max);
  //read value of ramping width
  configf.param("delt",delt);
  //read the value of total time
  //configf.param("t",t);
  //read the time at which time will reache half of its total range
  configf.param("t1",t1);
  //read the time at which time will come down half of its total range
  configf.param("t2",t2);
  
  

  

  // Check that t1 is less than t2, as ramping up must occurs first 
  if ( t1 > t2 ){
      cerr<<"t1 must be smalller than t2."<<endl;
      exit (EXIT_FAILURE);
  }
  
  
  // Check that t1 is not equal to t2 either.
  
  if ( t1 == t2 ){
      cerr<<"t1 and t2 must be  different."<<endl;
      exit (EXIT_FAILURE);
  }
  
  //Reading the temporal variation 
  for( int i=0; i<time_t; i++ )
  {
    ramp = (atan((i-t1)/delt)-atan((i-t2)/delt));
    deltanu.push_back(ramp);

  }
  
  //Find out the maximum and minimum value of the temporal variation 
    ramp_min = *std::min_element(deltanu.begin(), deltanu.end());
    ramp_max = *std::max_element(deltanu.begin(), deltanu.end());
   
    
  //std::cout << temp_min << endl;
  //std::cout << temp_max << endl;
  //std::cout << time_t << endl;
  
  n.clear(); n.resize(nodes,nu0);
  pos = (nu0>0)?1:-1;
  

  for( int i=0; i<nodes; i++ )
    P[i] = n[i]*prepropag.phiinit(configf);

  time = 0;
}

 
void CoupleAtan::step(void)
{
  time += deltat;
  for( int i=0; i<nodes; i++ )
           n[i] =nu0 + (nu_max-nu0) *(((atan((time-t1)/delt)-atan((time-t2)/delt))-ramp_min)/(ramp_max-ramp_min));
           
  Couple::step();
}

CoupleAtan::CoupleAtan( int nodes, double deltat, int index,
        const Propag& prepropag, const Population& postpop, double tempf )
    : Couple(nodes,deltat,index,prepropag,postpop)
{
  
  time_t = tempf;      
}

CoupleAtan::~CoupleAtan(void)
{
}