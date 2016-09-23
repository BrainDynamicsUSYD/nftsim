#ifndef NEUROFIELD_SRC_RANDOM_H
#define NEUROFIELD_SRC_RANDOM_H

#include<random>

class Random {
  Random(Random& ); // no copy constructor

  static int id; // Auto-increment the ID of each Random instance to use as the default seed

  std::mt19937_64 gen; // Generator
  std::normal_distribution<double> dist; // Distribution

 public:
  Random(double mean, double std);
  explicit Random(int seed, double mean = 0.0, double std = 1.0);
  void init(int seed, double mean, double std);
  double get(void);
  void get(double& target);
  ~Random(void);
};

#endif