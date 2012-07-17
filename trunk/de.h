#ifndef DIFFEQN
#define DIFFEQN

#include<vector>
using std::vector;

class RK4
{
  RK4(void);
  RK4(RK4&);
  void operator=(RK4&);
protected:
  int n; // dimension of system == y.size()
  int nodes;
  double deltat;
  double h6; // == deltat/6
  
  vector<double> k1;
  vector<double> k2;
  vector<double> k3;
  vector<double> k4;
  vector<double> temp;

  // define dydt here
  virtual void rhs( const vector<double>& y, vector<double>& dydt ) = 0;
public:
  vector<vector<double> > fields;

  RK4( int n, int nodes, double deltat )
    : n(n), nodes(nodes), deltat(deltat), h6(deltat/6.),
      k1(n), k2(n), k3(n), k4(n), temp(n),  fields(n) {
    for( int i=0; i<n; i++ )
      fields[i].resize(nodes);
  }
  virtual ~RK4(void) {}
  virtual vector<double>& operator[] ( int index )
    { return fields[index]; }
  virtual const vector<double>& operator[] ( int index ) const
    { return fields[index]; }

  virtual void step(void) {
    for( int j=0; j<nodes; j++ ) {
      for( int i=0; i<n; i++ )
        temp[i] = fields[i][j];
      rhs(temp,k1);
      for( int i=0; i<n; i++ )
        temp[i] = fields[i][j] +deltat*.5*k1[i];
      rhs(temp,k2);
      for( int i=0; i<n; i++ )
        temp[i] = fields[i][j] +deltat*.5*k2[i];
      rhs(temp,k3);
      for( int i=0; i<n; i++ )
        temp[i] = fields[i][j] +deltat*k3[i];
      rhs(temp,k4);
      for( int i=0; i<n; i++ )
        fields[i][j] += h6*( k1[i] +2*k2[i] +2*k3[i] +k4[i] );
    }
  }
};

#endif
