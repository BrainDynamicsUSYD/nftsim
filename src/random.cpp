/** @file random.cpp
  @brief Portable Gaussian random numbers for white-noise stimuli.

  Each Random object owns one std::mt19937_64 generator. Its output sequence
  is fixed by the C++ standard for a given seed, but the standard library's
  distributions (std::normal_distribution, std::uniform_real_distribution,
  std::generate_canonical) are not: libstdc++ and libc++ turn the same bits
  into different numbers. So the conversion from bits to Gaussian variates is
  written out here, and uses only operations whose results IEEE 754 fixes:

  1. Uniform variate: take the top 53 bits of a 64-bit output x, so
     u = (x >> 11) * 2^-53 lies on [0, 1), and map it to v = 2u - 1 on
     [-1, 1). Both steps are exact in double precision.
  2. Marsaglia polar method: draw v1, v2 as in step 1 and set
     s = v1*v1 + v2*v2, rejecting the pair and drawing again while s >= 1 or
     s == 0 (so log(0) never happens). On average 4/pi pairs are drawn.
     Then z1 = v1*f and z2 = v2*f, with f = sqrt(-2 log(s) / s), are two
     independent standard normal variates. z2 is returned first and z1 is
     cached for the next call.
  3. Scaling: get() returns mean + std*z, with the mean and standard
     deviation (std) given to the constructor. z is always finite, so
     std == 0 (eg a White stimulus with ASD: 0) returns exactly the mean;
     std::normal_distribution instead requires std > 0.

  The two multiply-adds (s and the scaling) use std::fma explicitly. fma is
  correctly rounded, so the result does not depend on whether the compiler
  contracts a*b + c into a fused multiply-add (clang does by default on
  arm64, gcc does with -ffp-contract=fast). Division and sqrt are correctly
  rounded by IEEE 754. The one remaining platform dependence is std::log,
  which comes from the system maths library. glibc and Apple's libm are both
  accurate to within 1 ulp, so they can differ in the last bit of log(s),
  which makes the variate differ by a few ulp (about 1 variate in 2000 when
  glibc 2.34 was compared with macOS), the same size as the round-off
  differences in the rest of the simulation.

  libstdc++ (GCC) and libc++ (clang) both implement std::normal_distribution
  with the polar method on a uniform variate 2u - 1, but libc++ returns z1
  first and libstdc++ returns z2 first, and their conversions of 64 bits to u
  differ from the one above only at the level of the last bit. The order
  used here is libstdc++'s, so for the same seed the noise agrees to
  round-off with that of earlier nftsim versions built with GCC (with which
  the regression references were made), and clang builds now give the same.

  Caching the second variate of each pair is deterministic because each
  Random object has its own generator and nothing else draws from it: for a
  given seed, the n-th call to get() always returns the same number. (White
  draws one variate per node per time step, WhiteCoherent one per time step;
  both just consume the sequence in order.) init() clears the cache along
  with reseeding the generator.

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "random.h" // Random;

// C++ standard library headers
#include <cmath>   // std::fma; std::log; std::sqrt;
#include <cstdint> // uint_fast64_t; int64_t;
#include <random>  // std::mt19937_64;

uint_fast64_t Random::id = 0;

Random::Random(double mean, double std) {
  init(++id,mean,std);
}

Random::Random(uint_fast64_t seed, double mean, double std) {
  init(seed,mean,std);
  ++id;
}

void Random::init(uint_fast64_t seed, double mean, double std) {
  gen = std::mt19937_64(seed); ///< Generator: Mersenne Twister, 64-bit.
  mu = mean;
  sigma = std;
  spare = 0.0;
  has_spare = false;
}

/** @brief Uniform variate on [-1, 1), on a grid of 2^-52.

  Uses the top 53 bits of one generator output, k = x >> 11 in [0, 2^53), and
  returns 2*(k * 2^-53) - 1 = (k - 2^52) * 2^-52, computed in integers and
  converted exactly to double.
*/
double Random::uniform_pm1() {
  const int64_t k = static_cast<int64_t>(gen() >> 11);
  return static_cast<double>(k - (INT64_C(1) << 52)) * (1.0 / 4503599627370496.0); // 2^-52
}

/** @brief Standard normal variate by Marsaglia's polar method.

  Returns the variate cached from the previous pair if there is one;
  otherwise draws a new pair, returns z2 = v2*f and caches z1 = v1*f.
*/
double Random::standard_normal() {
  if( has_spare ) {
    has_spare = false;
    return spare;
  }
  double v1;
  double v2;
  double s;
  do {
    v1 = uniform_pm1();
    v2 = uniform_pm1();
    s = std::fma(v1, v1, v2*v2);
  } while( s >= 1.0 || s == 0.0 );
  const double f = std::sqrt(-2.0 * std::log(s) / s);
  spare = v1 * f;
  has_spare = true;
  return v2 * f;
}

void Random::get(double& target) {
  target = std::fma(sigma, standard_normal(), mu);
}

double Random::get() {
  double temp;
  get(temp);
  return temp;
}

Random::~Random() = default;
