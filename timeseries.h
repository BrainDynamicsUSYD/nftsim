#ifndef TIMESERIES_H
#define TIMESERIES_H

#include<string>
using std::string;
#include<vector>
using std::vector;
#include<cmath>
#include"nf.h"

class Timeseries : public NF
{
  Timeseries(Timeseries&);
  Timeseries(void);
protected:
  void init( Configf& configf );
  void restart( Restartf& restartf ) {}
  void dump( Dumpf& dumpf ) const {}

  vector<Timeseries*> series;
  double t;
  double cease;
public:
  Timeseries( int nodes, double deltat, int index );
  virtual ~Timeseries(void);
  void step(void);
  virtual void fire( vector<double>& Q ) const;
};

struct Const : public Timeseries
{
  double mean;
  Const(int nodes,double deltat,int index) : Timeseries(nodes,deltat,index) {}
  void init( Configf& configf );
  void fire( vector<double>& Q ) const;
};

struct Pulse : public Timeseries
{
  double amp, width, period, pulses;
  Pulse(int nodes,double deltat,int index) : Timeseries(nodes,deltat,index) {}
  void init( Configf& configf );
  void fire( vector<double>& Q ) const;
};

#include"random.h"
struct White : public Timeseries
{
  double seed, amp, mean;
  Random* random;
  White(int nodes,double deltat,int index) : Timeseries(nodes,deltat,index) {}
  virtual ~White(void) { delete random; }
  virtual void init( Configf& configf );
  void fire( vector<double>& Q ) const;
};

struct WhiteCoherent : public Timeseries
{
  double seed, amp, mean;
  Random* random;
  WhiteCoherent(int nodes,double deltat,int index) : Timeseries(nodes,deltat,index) {}
  virtual ~WhiteCoherent(void) { delete random; }
  void init( Configf& configf );
  void fire( vector<double>& Q ) const;
};

struct WhiteFourier : public White
{
  double /*seed, amp, mean,*/deltax;
  //Random* random;
  WhiteFourier(int nodes,double deltat,int index) : White(nodes,deltat,index) {}
  virtual ~WhiteFourier(void) {}
  void init( Configf& configf );
};


struct PAS : public Timeseries
{
  double n20w, n20h, p25w, p25h, tmsw, tmsh, t_mns, isi;
  PAS(int nodes,double deltat,int index) : Timeseries(nodes,deltat,index) {}
  void init( Configf& configf );
  void fire( vector<double>& Q ) const;
};

#endif
