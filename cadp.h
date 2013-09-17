#ifndef CADP_H
#define CADP_H

#include"couple.h"
#include"de.h"

class Cascade
{
protected:
	int index; double deltat;
	double rise_t; double fall_t;
	double threshold; double drive;
	double m; double dmdt;
	double precascade_old;
public:
	Cascade( int index, double deltat, double rise_t, double fall_t, double threshold )
		: index(index), deltat(deltat),
		rise_t(rise_t), fall_t(fall_t), threshold(threshold),
		m(0), dmdt(0), precascade_old(0) {}
	virtual ~Cascade(void) {}
	virtual void step( double precascade ) {
		double source = precascade-precascade_old;
		drive = pow(source,2.2)/( pow(source,2) +pow(threshold,2) );
		precascade_old = 0;
		m    += deltat*dmdt; if( m<0 ) m=0;
		dmdt += deltat*( -(rise_t+fall_t)*dmdt +rise_t*fall_t*(drive-m) );
	}
	virtual operator double() const { return m; };
};

class Cascades : public Cascade
{
	double timescale, threshold;
	vector<Cascade*> cascade;
	vector<double> output;
public:
	Cascades( int num_cascades, double deltat, double timescale, double threshold )
		: Cascade(num_cascades,deltat,0,0,0),
		timescale(timescale), threshold(threshold), output(num_cascades) {
		for( int i=0; i<num_cascades; i++ )
			cascade.push_back( new Cascade(i,deltat,
				/*rise_t*/    pow((i+1)/timescale,.5),
				/*fall_t*/    pow((i+1)/timescale,.5),
				/*threshold*/ pow((i+1)/threshold,.7) ) );
	}
	~Cascades(void) {
		for( Cascade* pcascade: cascade )
			if(pcascade) delete pcascade;
	}
	void step( double precascade ) {
		cascade[0]->step(precascade);
		m = output[0] = *cascade[0];
		for( int i=1; i<index; i++ ) {
			cascade[i]->step( *cascade[i-1] );
			m        += *cascade[i];
			output[i] = *cascade[i];
		}
	}
	operator double() const { return m; };
};

class CaDP : public Couple
{
  CaDP();
  CaDP(CaDP&);
protected:
  Cascades* pot_agent; Cascade* dep_agent;
  vector<double> fpot_agent, fdep_agent;
  double nu_init;
  struct CaDE : public DE
  {
    double nu_init;
    double alpha; // for fCaDE
    double alpha_beta; // for fCaDE, == alpha -beta

    double B; // 1/stanard deviation of glutamate binding
    double glu_0; // glutamte dose-response threshold

    double max; // maximum synaptic strength
    double xth;  // threshold time-scale of plasticity
    double yth;  // threshold time-scale of plasticity
    double ltd; // time-scale of depression
    double ltp; // time-scale of potentiation

    double dth; // calcium threshold to depression
    double pth; // calcium threshold to potentiation

    double tCa; // time-scale of calcium influx/cascade

    double gnmda; // gain for NMDA receptor
    double z; // timescale of protein cascade

    double pos; // sign of nu

    virtual void init( Configf& configf );
    CaDE( int nodes, double deltat ) : DE(nodes,deltat,8) {}
    virtual ~CaDE(void) {}

    virtual void rhs( const vector<double>& y, vector<double>& dydt );
    double sig( double x, double beta ) const;
    virtual double _x( double Ca ) const; // potentiation rate
    virtual double _y( double Ca ) const; // depression rate
    virtual void pot(void);
    virtual void dep(void);
  };
  CaDE* de;
  RK4* rk4;
  virtual void init( Configf& configf );
public: 
  CaDP( int nodes, double deltat, int index,
          const Propag& prepropag, const Population& postpop );
  virtual ~CaDP(void);
  virtual void step(void);
  virtual void output( Output& output ) const;
};

#endif
