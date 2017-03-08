/** @file random.h
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

#ifndef NEUROFIELD_SRC_RANDOM_H
#define NEUROFIELD_SRC_RANDOM_H

// C++ standard library headers
#include <random> // std::mt19937_64; std::normal_distribution;

class Random {
  Random(const Random&); // No copy constructor allowed.

  static uint_fast64_t id; // Auto-increment the ID of each Random instance to use as the default seed

  std::mt19937_64 gen; ///< Generator
  std::normal_distribution<double> dist; ///< Distribution

 public:
  Random(double mean, double std);
  explicit Random(uint_fast64_t seed, double mean = 0.0, double std = 1.0);
  void init(uint_fast64_t seed, double mean, double std);
  double get(void);
  void get(double& target);
  ~Random(void);
};

#endif // NEUROFIELD_SRC_RANDOM_H
