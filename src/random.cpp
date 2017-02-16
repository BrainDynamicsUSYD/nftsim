/** @file random.cpp
  @brief A brief, one sentence description.

  A more detailed multiline description...

  @author Peter Drysdale, Felix Fung,
*/

// Neurofield headers
#include "random.h" // Random;

uint_fast64_t Random::id = 0;

Random::Random(double mean, double std) {
  init(++id,mean,std);
}

Random::Random(uint_fast64_t seed, double mean, double std) {
  init(seed,mean,std);
  ++id;
}

void Random::init(uint_fast64_t seed, double mean, double std) {
  gen = std::mt19937_64(seed); ///< Generator
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
