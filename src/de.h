/** @file de.h
  @brief A collection of classes for differential equations and their integrators.

  The base differential equation class (DE) requires specific differential
  equation classes, derived from DE, to specify their equations in DE::rhs.
  There are currently two integrators derived from the base Integrator class,
  namely Euler and RK4.

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_DE_H
#define NFTSIM_SRC_DE_H

// C++ standard library headers
#include <vector> // std::vector;

using vvd_size_type = std::vector<std::vector<double>>::size_type;

/// Base class for differential equations.
class DE {
 protected:
  /// if the number of field variables need to be extended, use this function.
  void extend( vvd_size_type extension ) {
    ndim += extension;
    variables.resize(ndim);
    for( vvd_size_type i=0; i<ndim; i++ ) {
      variables[i].resize(nodes,0);
    }
  }
 public:
  DE() = delete;          ///< No default constructor allowed.
  DE(const DE&) = delete; ///< No copy constructor allowed.
  void operator=(const DE&) = delete; ///< No copy assignment allowed.

  vvd_size_type nodes; ///< number of nodes in the system.
  double deltat;       ///< integration timestep size [s].
  vvd_size_type ndim;  ///< dimension of system == y.size()
  std::vector<std::vector<double> > variables;

  DE( vvd_size_type nodes, double deltat, vvd_size_type ndim )
    : nodes(nodes), deltat(deltat), ndim(ndim), variables(ndim) {
    for( vvd_size_type i=0; i<ndim; i++ ) {
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
  /// Classes derived from DE, define their specific differential equations here.
  virtual void rhs( const std::vector<double>& y, std::vector<double>& dydt, std::vector<double>::size_type n ) = 0;
};

/// Base class for bringing together differential equations (DE) and integration schemes Integrator::step().
class Integrator {
 protected:
  DE& de;
 public:
  Integrator() = delete;                  ///< No default constructor allowed.
  Integrator(const Integrator&) = delete; ///< No copy constructor allowed.
  void operator=(const Integrator&) = delete; ///< No copy assignment allowed.

  explicit Integrator( DE& de ) : de(de) {}
  virtual ~Integrator() = default;
  virtual void step() = 0; ///< Scheme for advancing de one step forward in time. Pure virtual, must be overridden.
};

/// Implements the Euler integration scheme.
class Euler : public Integrator {
 protected:
  std::vector<double> dydt;
 public:
  Euler() = delete;             ///< No default constructor allowed.
  Euler(const Euler&) = delete; ///< No copy constructor allowed.
  void operator=(const Euler&) = delete; ///< No copy assignment allowed.

  explicit Euler( DE& de ) : Integrator(de), dydt(de.ndim) {}
  ~Euler() override = default;
  void step() override {
    for( vvd_size_type j=0; j<de.nodes; j++ ) {
      de.rhs( de.variables[j], dydt, j );
      for( vvd_size_type i=0; i<de.ndim; i++ ) {
        de.variables[i][j] += dydt[i]*de.deltat;
      }
    }
  }
};

/// Implements the fourth order Runge-Kutta integration scheme.
class RK4 : public Integrator {
 protected:
  double h6; ///< == deltat/6
  double deltat5; ///< == deltat*0.5

  std::vector<double> k1;
  std::vector<double> k2;
  std::vector<double> k3;
  std::vector<double> k4;
  std::vector<double> temp;
 public:
  RK4() = delete;           ///< No default constructor allowed.
  RK4(const RK4&) = delete; ///< No copy constructor allowed.
  void operator=(const RK4&) = delete; ///< No copy assignment allowed.

  explicit RK4( DE& de ) : Integrator(de), h6(de.deltat/6.0), deltat5(de.deltat*0.5),
    k1(de.ndim), k2(de.ndim), k3(de.ndim), k4(de.ndim), temp(de.ndim) {}
  ~RK4() override = default;

  /// Implements the 4th order Runge-Kutta scheme for advancing Integrator::de one step forward in time.
  void step() override {
    for( vvd_size_type j=0; j<de.nodes; j++ ) {
      for( vvd_size_type i=0; i<de.ndim; i++ ) {
        temp[i] = de.variables[i][j];
      }
      de.rhs(temp, k1, j);
      for( vvd_size_type i=0; i<de.ndim; i++ ) {
        temp[i] = de.variables[i][j] + (deltat5 * k1[i]);
      }
      de.rhs(temp, k2, j);
      for( vvd_size_type i=0; i<de.ndim; i++ ) {
        temp[i] = de.variables[i][j] + (deltat5 * k2[i]);
      }
      de.rhs(temp, k3, j);
      for( vvd_size_type i=0; i<de.ndim; i++ ) {
        temp[i] = de.variables[i][j] + (de.deltat * k3[i]);
      }
      de.rhs(temp, k4, j);
      for( vvd_size_type i=0; i<de.ndim; i++ ) {
        de.variables[i][j] += h6*(k1[i] + (2.0 * k2[i]) + (2.0 * k3[i]) + k4[i]);
      }
    }
  }
};

#endif //NFTSIM_SRC_DE_H
