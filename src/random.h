/** @file random.h
  @brief Portable Gaussian random numbers for white-noise stimuli.

  A Random object draws normally distributed variates with a given mean and
  standard deviation. The uniform bits come from std::mt19937_64, whose output
  sequence is fixed by the C++ standard. The Gaussian transform (Marsaglia's
  polar method) is written out in random.cpp instead of using
  std::normal_distribution, whose algorithm is implementation-defined, so the
  same seed gives the same noise with libstdc++ (GNU) and libc++ (clang).

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NFTSIM_SRC_RANDOM_H
#define NFTSIM_SRC_RANDOM_H

// C++ standard library headers
#include <cstdint> // uint_fast64_t;
#include <random>  // std::mt19937_64;

class Random {
  static uint_fast64_t id; // Auto-increment the ID of each Random instance to use as the default seed

  std::mt19937_64 gen; ///< Generator: 64-bit Mersenne Twister.
  double mu = 0.0;     ///< Mean of the variates returned by get().
  double sigma = 1.0;  ///< Standard deviation of the variates returned by get().
  double spare = 0.0;  ///< Cached standard normal variate (z1 of the last polar pair).
  bool has_spare = false; ///< True when spare holds a variate not yet returned.

  double uniform_pm1();     // Uniform variate on [-1, 1), on a grid of 2^-52.
  double standard_normal(); // Standard normal variate (mean 0, standard deviation 1).

 public:
  Random(const Random&) = delete; // No copy constructor allowed.

  Random(double mean, double std);
  explicit Random(uint_fast64_t seed, double mean = 0.0, double std = 1.0);
  void init(uint_fast64_t seed, double mean, double std);
  double get();
  void get(double& target);
  ~Random();
};

#endif // NFTSIM_SRC_RANDOM_H
