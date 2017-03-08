/** @file de.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_DE_H
#define NEUROFIELD_SRC_DE_H

// C++ standard library headers
#include <vector> // std::vector;

typedef std::vector<std::vector<double>>::size_type vvd_size_type;

class DE {
  DE();          // No default constructor allowed.
  DE(const DE&); // No copy constructor allowed.
  void operator=(DE&);
 protected:
  // if the number of field variables need to be extended, use this function
  void extend( vvd_size_type extension ) {
    n += extension;
    variables.resize(n);
    for( vvd_size_type i=0; i<n; i++ ) {
      variables[i].resize(nodes,0);
    }
  }
 public:
  vvd_size_type nodes; ///< number of nodes in the system.
  double deltat;       ///< integration timestep size.
  vvd_size_type n;     ///< dimension of system == y.size()
  std::vector<std::vector<double> > variables;

  DE( vvd_size_type nodes, double deltat, vvd_size_type n )
    : nodes(nodes), deltat(deltat), n(n), variables(n) {
    for( vvd_size_type i=0; i<n; i++ ) {
      variables[i].resize(nodes);
    }
  }
  virtual ~DE() = default;

  virtual std::vector<double>& operator[] ( vvd_size_type index ) {
    return variables[index];
  }
  virtual const std::vector<double>& operator[] ( vvd_size_type index ) const {
    return variables[index];
  }
  // define dydt here
  virtual void rhs( const std::vector<double>& y, std::vector<double>& dydt ) = 0;
};

class Integrator {
  Integrator();
  Integrator(Integrator&);
  void operator=(Integrator&);
 protected:
  DE& de;
 public:
  explicit Integrator( DE& de ) : de(de) {}
  virtual ~Integrator() = default;
  virtual void step() = 0;
};

class Euler : public Integrator {
  Euler();
  Euler(Euler&);
  void operator=(Euler&);
 protected:
  std::vector<double> dydt;
 public:
  explicit Euler( DE& de ) : Integrator(de), dydt(de.n) {}
  ~Euler() override = default;
  void step() override {
    for( vvd_size_type j=0; j<de.nodes; j++ ) {
      de.rhs( de.variables[j], dydt );
      for( vvd_size_type i=0; i<de.n; i++ ) {
        de.variables[i][j] += dydt[i]*de.deltat;
      }
    }
  }
};

class RK4 : public Integrator {
  RK4();
  RK4(RK4&);
  void operator=(RK4&);
 protected:
  double h6; ///< == deltat/6
  double deltat5; ///< == deltat*0.5

  std::vector<double> k1;
  std::vector<double> k2;
  std::vector<double> k3;
  std::vector<double> k4;
  std::vector<double> temp;
 public:
  explicit RK4( DE& de ) : Integrator(de), h6(de.deltat/6.), deltat5(de.deltat*0.5),
    k1(de.n), k2(de.n), k3(de.n), k4(de.n), temp(de.n) {}
  ~RK4() override = default;

  void step() override {
    for( vvd_size_type j=0; j<de.nodes; j++ ) {
      for( vvd_size_type i=0; i<de.n; i++ ) {
        temp[i] = de.variables[i][j];
      }
      de.rhs(temp,k1);
      for( vvd_size_type i=0; i<de.n; i++ ) {
        temp[i] = de.variables[i][j] +deltat5*k1[i];
      }
      de.rhs(temp,k2);
      for( vvd_size_type i=0; i<de.n; i++ ) {
        temp[i] = de.variables[i][j] +deltat5*k2[i];
      }
      de.rhs(temp,k3);
      for( vvd_size_type i=0; i<de.n; i++ ) {
        temp[i] = de.variables[i][j] +de.deltat*k3[i];
      }
      de.rhs(temp,k4);
      for( vvd_size_type i=0; i<de.n; i++ ) {
        de.variables[i][j] += h6*( k1[i] +2.0*k2[i] +2.0*k3[i] +k4[i] );
      }
    }
  }
};

#endif //NEUROFIELD_SRC_DE_H
