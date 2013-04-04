#include<iostream>
using std::cerr;
using std::endl;
#include"timeseries.h"

void Timeseries::init( Configf& configf )
{
  int superimpose = 1; configf.optional("Superimpose",superimpose);
  for( int i=0; i<superimpose; i++ ) {
    vector<string> mode = configf.arb("-");
    configf.optional("Onset",t); t = -t;
    if( !configf.optional("Duration",cease) )
      if( configf.optional("Cease",cease) )
        cease += t;
    // PUT YOUR TIMEFUNCTION HERE
    if( mode[0]=="Const" )
      series.push_back( new TIMESERIES::Const(nodes,deltat,index) );
    else if( mode[0]=="Pulse" )
      series.push_back( new TIMESERIES::Pulse(nodes,deltat,index) );
    else if( mode[0]=="White" )
      series.push_back( new TIMESERIES::White(nodes,deltat,index) );
    else if( mode[0]=="WhiteCoherent" )
      series.push_back( new TIMESERIES::WhiteCoherent(nodes,deltat,index) );
    else if( mode[0]=="PAS" )
      series.push_back( new TIMESERIES::PAS(nodes,deltat,index) );
    else if( mode[0]=="Burst" )
      series.push_back( new TIMESERIES::Burst(nodes,deltat,index) );
	else {
      cerr<<"Stimulus mode "<<mode[0].c_str()<<" not found"<<endl;
      exit(EXIT_FAILURE);
    }
    // END PUT YOUR TIMEFUNCTION HERE
    series[i]->t = t; series[i]->cease = cease;
    series[i]->init(configf);
  }

}

Timeseries::Timeseries( int nodes, double deltat, int index )
    : NF(nodes,deltat,index), series(), t(0), cease(1000)
{
}

Timeseries::~Timeseries(void)
{
  for( size_t i=0; i<series.size(); i++ )
    delete series[i];
}

void Timeseries::fire( vector<double>& Q ) const
{
  static vector<double> temp;
  temp.clear(); temp.resize(nodes,0);
  Q.clear(); Q.resize(nodes,0);
  for( size_t i=0; i<series.size(); i++ )
    if( series[i]->t>=0 && series[i]->t<series[i]->cease ) {
      series[i]->fire(temp);
      for( int i=0; i<nodes; i++ )
        Q[i] += temp[i];
    }
}

void Timeseries::step(void)
{
  for( size_t i=0; i<series.size(); i++ )
    series[i]->t += deltat;
}

namespace TIMESERIES
{

void Const::init( Configf& configf )
{
  // Mean: 0
  configf.param("Mean",mean);
}

void Const::fire( vector<double>& Q ) const
{
  for( int i=0; i<nodes; i++ )
    Q[i] = mean;
}

void Pulse::init( Configf& configf )
{
  period = 1000; pulses = 1;
  // Amplitude: 1 Width: .5e-3 "Period/Frequency": 1 "Pulses": 1
  configf.param("Amplitude",amp);
  configf.param("Width",width);
  if( !configf.optional("Period",period) )
    if( configf.optional("Frequency",period) )
      period = 1/period;
  configf.optional("Pulses",pulses);
}

void Pulse::fire( vector<double>& Q ) const
{
  if( fmod(t,period)>=0 && fmod(t,period)<width && t/period<pulses )
    for( int i=0; i<nodes; i++ )
      Q[i] = amp;
}

void White::init( Configf& configf )
{
  // "Ranseed": 0 Amplitude: 1 Mean: 1
  seed = -98715872;
  configf.optional("Ranseed",seed);
  random = new Random(seed);
  configf.param("Amplitude",amp);
  configf.param("Mean",mean);
    if(configf.optional("Deltax",deltax)) // If deltax is given, rescale amp
        amp = sqrt(4*pow(M_PI,3)*pow(amp,2)/deltat/deltax/deltax);
}

void White::fire( vector<double>& Q ) const
{
  double deviate1, deviate2;
  for( int i=0; i<nodes-1; i+= 2 ) {
    random->gaussian(deviate1,deviate2);
    Q[i]   = amp*deviate1 +mean;
    Q[i+1] = amp*deviate2 +mean;
  }
  if( nodes%2 ) {
    random->gaussian(deviate1,deviate2);
    Q[nodes-1] = amp*deviate1 +mean;
  }
}

void WhiteCoherent::init( Configf& configf )
{
  // "Ranseed": 0 Amplitude: 1 Mean: 1
  seed = -98715872;
  configf.optional("Ranseed",seed);
  random = new Random(seed);
  configf.param("Amplitude",amp);
  configf.param("Mean",mean);
}

void WhiteCoherent::fire( vector<double>& Q ) const
{
  double deviate1, deviate2;
  random->gaussian(deviate1,deviate2);
  for( int i=0; i<nodes; i++ )
    Q[i] = amp*deviate1 +mean;
}

void PAS::init( Configf& configf )
{
  // ISI: 10e-3
  // N20 width: 2.5e-3 N20 height: 5
  // P25 width: 3.5e-3 P25 height: 5
  // TMS width: 0.5e-3 TMS height: 3
  configf.param("ISI",isi);
  configf.param("N20 width", n20w);
  configf.param("N20 height",n20h);
  configf.param("P25 width", p25w);
  configf.param("P25 height",p25h);
  configf.param("TMS width", tmsw);
  configf.param("TMS height",tmsh);
  if( isi<0 ) {
    t -= isi;
    t_mns = -isi;
  }
  else {
    t_mns = 0;
  }
}

void PAS::fire( vector<double>& Q ) const
{
  // MNS
  if( t_mns<=t && t<t_mns+n20w )
    for( int i=0; i<nodes; i++ )
      Q[i] = -n20h*sin(3.14159*(t-t_mns)/n20w);
  else if( t_mns+n20w<=t && t<t_mns+n20w+p25w )
    for( int i=0; i<nodes; i++ )
      Q[i] =  p25h*sin(3.14159*(t-t_mns-n20w)/p25w);

  // TMS
  if( t_mns+n20w/2+isi<=t && t<t_mns+n20w/2+isi+tmsw )
    for( int i=0; i<nodes; i++ )
      Q[i] += tmsh;
}

void Burst::init( Configf& configf )
{
  // Amplitude: 10 Width: .5e-3 Bursts: 3 Burst Frequency: 50 On: 2 Off: 8 Total Pulses: 1000
  configf.param("Amplitude",amp);
  configf.param("Width",width);
  configf.param("Bursts",bursts);
  configf.param("Burst Frequency",freq);
  configf.param("Oscillation Frequency",oscillation_freq);
  configf.param("On",on);
  configf.param("Off",off);
}

void Burst::fire( vector<double>& Q ) const
{
  if( fmod(t,on+off)>=0 && fmod(t,on+off)<on &&
      fmod(t,1/oscillation_freq)>=0 && fmod(t,1/oscillation_freq)<bursts/freq &&
      fmod(t,1/freq)>=0 && fmod(t,1/freq)<width )
    for( int i=0; i<nodes; i++ )
      Q[i] = amp;
}

} // this bracket closes namespace TIMESERIES, do not touch
