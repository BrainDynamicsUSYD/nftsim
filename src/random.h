#ifndef RANDOM_H
#define RANDOM_H

#include<random>

class Random
{
  Random(Random& ); // no copy constructor

  static int id; // Auto-increment the ID of each Random instance to use as the default seed

  std::mt19937_64 gen; // Generator
  std::normal_distribution<double> dist; // Distribution

public: 
  Random(double mean, double std);
  Random(int seed,double mean, double std);
  void init(int seed,double mean, double std);
  double get(void);
  void get(double& target);
  ~Random(void);
};

#endif