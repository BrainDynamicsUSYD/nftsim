/** @file random.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Main module header
#include "random.h" // Random;

// C++ standard library headers
#include <random> // std::mt19937_64; std::normal_distribution;

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
  dist = std::normal_distribution<double>(mean,std); ///< Distribution
}

void Random::get(double& target) {
  target = dist(gen);
}

double Random::get() {
  double temp;
  get(temp);
  return temp;
}

Random::~Random() = default;
