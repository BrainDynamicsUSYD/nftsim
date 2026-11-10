/** @file steady_state.cpp
  @brief Defines checkSteadyState(), which warns if the initial state is not a steady state.

  See steady_state.h for the steady-state conditions that are checked.
*/

// Main module header
#include "steady_state.h"

// Other nftsim headers
#include "coupling.h"   // Coupling;
#include "population.h" // Population;
#include "propagator.h" // Propagator;

// C++ standard library headers
#include <algorithm> // std::max; std::swap;
#include <cmath>     // std::fabs; std::isfinite; std::sqrt;
#include <cstddef>   // std::size_t;
#include <ios>       // std::scientific;
#include <iomanip>   // std::setprecision;
#include <ostream>   // std::ostream; std::endl;
#include <sstream>   // std::ostringstream;
#include <string>    // std::string;
#include <vector>    // std::vector;
using std::endl;
using std::fabs;
using std::max;
using std::size_t;
using std::string;
using std::vector;

namespace steady_state {

namespace {

/// Relative mismatch between x and y, 0 if both are 0.
double mismatch( double x, double y ) {
  const double scale = max(fabs(x), fabs(y));
  return scale > 0.0 ? fabs(x - y)/scale : 0.0;
}

/// True if every element of x equals x[0] to round-off.
bool uniform( const vector<double>& x ) {
  for( double xi : x ) {
    if( fabs(xi - x[0]) > 1e-12*fabs(x[0]) ) {
      return false;
    }
  }
  return true;
}

/// Solves A x = b in place (b becomes x) by Gaussian elimination with partial
/// pivoting. Returns false if A is singular.
bool solve( vector< vector<double> > A, vector<double>& b ) {
  const size_t n = b.size();
  for( size_t k=0; k<n; k++ ) {
    size_t pivot = k;
    for( size_t i=k+1; i<n; i++ ) {
      if( fabs(A[i][k]) > fabs(A[pivot][k]) ) {
        pivot = i;
      }
    }
    if( !(fabs(A[pivot][k]) > 0.0) ) {
      return false;
    }
    std::swap(A[k], A[pivot]);
    std::swap(b[k], b[pivot]);
    for( size_t i=k+1; i<n; i++ ) {
      const double factor = A[i][k]/A[k][k];
      for( size_t j=k; j<n; j++ ) {
        A[i][j] -= factor*A[k][j];
      }
      b[i] -= factor*b[k];
    }
  }
  for( size_t k=n; k-- > 0; ) {
    for( size_t j=k+1; j<n; j++ ) {
      b[k] -= A[k][j]*b[j];
    }
    b[k] /= A[k][k];
  }
  return true;
}

/** @brief The spatially uniform steady-state equations, Q_a = S_a(sum_b nu_ab Q_b).

  `nu[c]` is the effective coupling strength of connection c, and the
  unknowns are the firing rates of the neural populations (`neural` lists
  their indices); stimulus populations keep their initial Q.
*/
struct Network {
  const std::vector<const Population*>& pops;
  const std::vector<Connection>& connections;
  vector<double> nu;
  vector<size_t> neural;

  /// Residual F_a = S_a(V_a) - Q_a and Jacobian dF/dQ, at the neural Q `x`.
  bool residual( const vector<double>& x, vector<double>& F,
                 vector< vector<double> >* J ) const {
    vector<double> Q(pops.size());
    vector<long> unknown(pops.size(), -1);
    for( size_t p=0; p<pops.size(); p++ ) {
      Q[p] = pops[p]->initialQ();
    }
    for( size_t k=0; k<neural.size(); k++ ) {
      Q[neural[k]] = x[k];
      unknown[neural[k]] = static_cast<long>(k);
    }
    const size_t n = neural.size();
    F.assign(n, 0.0);
    if( J != nullptr ) {
      J->assign(n, vector<double>(n, 0.0));
    }
    for( size_t k=0; k<n; k++ ) {
      double v = 0.0;
      for( size_t c=0; c<connections.size(); c++ ) {
        if( connections[c].post == neural[k] ) {
          v += nu[c]*Q[connections[c].pre];
        }
      }
      double q, dqdv;
      if( !pops[neural[k]]->steadyFiring(v, q, dqdv) ) {
        return false;
      }
      F[k] = q - x[k];
      if( J != nullptr ) {
        for( size_t c=0; c<connections.size(); c++ ) {
          if( connections[c].post == neural[k] && unknown[connections[c].pre] >= 0 ) {
            (*J)[k][static_cast<size_t>(unknown[connections[c].pre])] += dqdv*nu[c];
          }
        }
        (*J)[k][k] -= 1.0;
      }
    }
    for( size_t k=0; k<n; k++ ) {
      if( !std::isfinite(F[k]) ) {
        return false;
      }
    }
    return true;
  }

  static double norm( const vector<double>& F ) {
    double sum = 0.0;
    for( double f : F ) {
      sum += f*f;
    }
    return std::sqrt(sum);
  }

  /** @brief Newton's method with a backtracking line search, from `x`.

    Converges to a steady state near the configured firing rates if the
    Jacobian there is not singular; a sigmoid network may have several steady
    states, and this one is not necessarily stable. Returns false, rather
    than a wrong value, if it does not converge in 100 iterations.
  */
  bool newton( vector<double>& x ) const {
    vector<double> F, Fnew, dx;
    vector< vector<double> > J;
    if( !residual(x, F, &J) ) {
      return false;
    }
    for( int iteration=0; iteration<100; iteration++ ) {
      bool converged = true;
      for( size_t k=0; k<x.size(); k++ ) {
        if( fabs(F[k]) > 1e-12*(fabs(x[k]) + fabs(F[k] + x[k])) + 1e-14 ) {
          converged = false;
        }
      }
      if( converged ) {
        return true;
      }
      dx = F;
      for( double& d : dx ) {
        d = -d;
      }
      if( !solve(J, dx) ) {
        return false;
      }
      // Halve the step until the residual decreases.
      const double norm0 = norm(F);
      double lambda = 1.0;
      vector<double> xnew(x.size());
      bool accepted = false;
      for( int halving=0; halving<40 && !accepted; halving++, lambda *= 0.5 ) {
        for( size_t k=0; k<x.size(); k++ ) {
          xnew[k] = x[k] + lambda*dx[k];
        }
        accepted = residual(xnew, Fnew, nullptr) && norm(Fnew) < (1.0 - 1e-4*lambda)*norm0;
      }
      if( !accepted ) {
        return false;
      }
      x = xnew;
      residual(x, F, &J);
    }
    return false;
  }
};

} // namespace

bool close( double x, double y ) {
  return fabs(x - y) <= tolerance*max(fabs(x), fabs(y)) + abs_tolerance;
}

Result checkSteadyState( const std::vector<const Population*>& pops,
                         const std::vector<Connection>& connections,
                         std::ostream& warnings ) {
  Result result;
  result.populations.resize(pops.size());
  result.phi_consistent.assign(connections.size(), true);
  bool warned = false;
  // Report values as written in configuration files (10 significant figures,
  // as in the eirs configurations); stream state is restored afterwards.
  const std::streamsize saved_precision = warnings.precision();
  const std::ios_base::fmtflags saved_flags = warnings.flags();

  // Each neural population: Q implied by its initial inputs, node by node.
  for( size_t p=0; p<pops.size(); p++ ) {
    PopulationResult& r = result.populations[p];
    r.configured = pops[p]->initialQ();
    if( pops[p]->isStimulus() ) {
      continue; // its Q is its stimulus's
    }
    vector<double> v;
    for( const Connection& c : connections ) {
      if( c.post == p ) {
        const vector<double>& P = c.coupling->nuphi();
        v.resize(P.size(), 0.0);
        for( size_t i=0; i<P.size(); i++ ) {
          v[i] += P[i];
        }
      }
    }
    if( v.empty() ) { // no inputs: the soma potential is 0
      v.assign(1, 0.0);
    }
    double q, dqdv;
    if( !pops[p]->steadyFiring(v[0], q, dqdv) ) {
      continue; // BurstResponse: no steady state in V alone
    }
    r.checked = true;
    const double q0 = q;
    double worst = -1.0;
    for( size_t i=0; i<v.size(); i++ ) {
      pops[p]->steadyFiring(v[i], q, dqdv);
      if( fabs(q - q0) > 1e-12*fabs(q0) ) {
        r.uniform = false;
      }
      if( !close(q, r.configured) ) {
        r.consistent = false;
      }
      if( mismatch(q, r.configured) > worst ) { // the node of largest mismatch
        worst = mismatch(q, r.configured);
        r.node = i;
        r.v = v[i];
        r.implied = q;
      }
    }
    if( !r.consistent ) {
      warned = true;
      warnings << std::setprecision(10)
               << "WARNING: Population " << p+1 << "'s initial Q (" << r.configured
               << ") is not a steady state: its initial inputs give Q = "
               << std::setprecision(6) << r.implied;
      if( !r.uniform ) {
        warnings << " at node " << r.node+1;
      }
      warnings << " (soma potential " << std::scientific << std::setprecision(3)
               << r.v << " V)";
      if( !r.uniform ) {
        warnings << ", the node where it differs most";
      }
      warnings << ". The simulation will start with a transient." << endl;
      warnings.flags(saved_flags);
    }
  }

  // Each propagator: phi relaxes to the presynaptic Q, so starts there only
  // if its initial phi (`phi:`, Steady by default) equals that Q.
  for( size_t c=0; c<connections.size(); c++ ) {
    const vector<double>& phi = connections[c].propagator->phi();
    const double Qb = pops[connections[c].pre]->initialQ();
    size_t node = 0;
    double worst = -1.0;
    for( size_t i=0; i<phi.size(); i++ ) {
      if( !close(phi[i], Qb) ) {
        result.phi_consistent[c] = false;
      }
      if( mismatch(phi[i], Qb) > worst ) {
        worst = mismatch(phi[i], Qb);
        node = i;
      }
    }
    if( !result.phi_consistent[c] ) {
      warned = true;
      warnings << std::setprecision(10)
               << "WARNING: Propagator " << c+1 << "'s initial phi (" << phi[node];
      if( !uniform(phi) ) {
        warnings << " at node " << node+1 << ", where it differs most";
      }
      warnings << ") is not a steady state: it relaxes towards Population "
               << connections[c].pre+1 << "'s Q (" << Qb
               << "). The simulation will start with a transient." << endl;
    }
  }

  if( !warned ) {
    warnings.precision(saved_precision);
    return result;
  }

  // Seek a consistent set of Q, for a spatially uniform network. The
  // effective nu of each connection is its initial nu*phi over its initial
  // phi, which holds for every coupling type when both are uniform.
  Network network{pops, connections, vector<double>(connections.size(), 0.0), {}};
  for( size_t p=0; p<pops.size() && result.not_searched_reason.empty(); p++ ) {
    if( !pops[p]->isStimulus() ) {
      if( !result.populations[p].checked ) {
        std::ostringstream reason;
        reason << "Population " << p+1 << "'s firing response has no steady state"
               << " that depends on its soma potential alone";
        result.not_searched_reason = reason.str();
      }
      network.neural.push_back(p);
    }
  }
  for( size_t c=0; c<connections.size() && result.not_searched_reason.empty(); c++ ) {
    const vector<double>& phi = connections[c].propagator->phi();
    const vector<double>& P = connections[c].coupling->nuphi();
    std::ostringstream reason;
    if( !uniform(phi) || !uniform(P) ) {
      reason << "Coupling " << c+1 << "'s initial input nu*phi varies across nodes";
    } else if( phi[0] != 0.0 ) {
      network.nu[c] = P[0]/phi[0];
    } else if( !(pops[connections[c].pre]->isStimulus()
                 && pops[connections[c].pre]->initialQ() == 0.0) ) {
      reason << "Propagator " << c+1 << "'s initial phi is 0, so Coupling "
             << c+1 << "'s nu cannot be inferred from its initial nu*phi";
    }
    result.not_searched_reason = reason.str();
  }

  if( result.not_searched_reason.empty() ) {
    result.searched = true;
    vector<double> x;
    for( size_t p : network.neural ) {
      x.push_back(pops[p]->initialQ());
    }
    result.converged = network.newton(x);
    if( result.converged ) {
      result.suggested.resize(pops.size());
      for( size_t p=0; p<pops.size(); p++ ) {
        result.suggested[p] = pops[p]->initialQ();
      }
      for( size_t k=0; k<x.size(); k++ ) {
        result.suggested[network.neural[k]] = x[k];
      }
      warnings << "    Newton's method from the configured values converges to this"
               << " steady state (its stability is not checked):" << endl
               << std::setprecision(10);
      for( size_t p : network.neural ) {
        warnings << "      Population " << p+1 << ": Q: " << result.suggested[p] << endl;
      }
    } else {
      warnings << "    Newton's method from the configured values did not converge,"
               << " so no consistent set of Q values is suggested." << endl;
    }
  } else {
    warnings << "    No consistent set of Q values is suggested: "
             << result.not_searched_reason << "." << endl;
  }
  warnings.precision(saved_precision);
  warnings.flags(saved_flags);
  return result;
}

} // namespace steady_state
