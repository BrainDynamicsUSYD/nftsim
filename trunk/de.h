#ifndef DIFFEQN_H
#define DIFFEQN_H

#include<vector>
using std::vector;

class DE
{
  DE(void);
  DE(DE&);
  void operator=(DE&);
public:
  int nodes;
  double deltat;
  int n; // dimension of system == y.size()

  vector<vector<double> > variables;

  DE( int nodes, double deltat, int n )
      : nodes(nodes), deltat(deltat), n(n), variables(n) {
    for( int i=0; i<n; i++ )
      variables[i].resize(nodes);
  }
  ~DE(void) {}

  virtual vector<double>& operator[] ( int index )
    { return variables[index]; }
  virtual const vector<double>& operator[] ( int index ) const
    { return variables[index]; }

  // define dydt here
  virtual void rhs( const vector<double>& y, vector<double>& dydt ) = 0;
};

class Integrator
{
  Integrator(void);
  Integrator(Integrator&);
  void operator=(Integrator&);
protected:
  DE& de;
public:
  Integrator( DE& de ) : de(de) {}
  virtual ~Integrator(void) {}
  virtual void step(void) = 0;
};

class RK4 : public Integrator
{
  RK4(void);
  RK4(RK4&);
  void operator=(RK4&);
protected:
  double h6; // == deltat/6
  
  vector<double> k1;
  vector<double> k2;
  vector<double> k3;
  vector<double> k4;
  vector<double> temp;
public:
  RK4( DE& de ) : Integrator(de), h6(de.deltat/6.),
      k1(de.n), k2(de.n), k3(de.n), k4(de.n), temp(de.n) {}
  virtual ~RK4(void) {}

  virtual void step(void) {
    for( int j=0; j<de.nodes; j++ ) {
      for( int i=0; i<de.n; i++ )
        temp[i] = de.variables[i][j];
      de.rhs(temp,k1);
      for( int i=0; i<de.n; i++ )
        temp[i] = de.variables[i][j] +de.deltat*.5*k1[i];
      de.rhs(temp,k2);
      for( int i=0; i<de.n; i++ )
        temp[i] = de.variables[i][j] +de.deltat*.5*k2[i];
      de.rhs(temp,k3);
      for( int i=0; i<de.n; i++ )
        temp[i] = de.variables[i][j] +de.deltat*k3[i];
      de.rhs(temp,k4);
      for( int i=0; i<de.n; i++ )
        de.variables[i][j] += h6*( k1[i] +2*k2[i] +2*k3[i] +k4[i] );
    }
  }
};

#endif
