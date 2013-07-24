#include "random.h"

int Random::id = 0;

Random::Random(double mean = 0.0, double std = 1.0){
  init(++id,mean,std);
}

Random::Random(int seed,double mean = 0.0, double std = 1.0){
  init(seed,mean,std);
  ++id;
}

void Random::init(int seed,double mean, double std){
  gen = std::mt19937_64(seed); // Generator
  dist = std::normal_distribution<double>(mean,std); // Distribution
}

void Random::get(double& target){
  target = dist(gen);
}

double Random::get(void){
  double temp;
  get(temp);
  return temp;
}

Random::~Random()
{
}