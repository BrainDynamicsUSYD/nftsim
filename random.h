#ifndef RANDOM_H
#define RANDOM_H

class Random
{
  Random(Random& ); // no copy constructor

  // Use "enum hack" to obtain more portable static const int
  enum {IA=16807};
  enum {IM=2147483647};
  enum {IQ=127773};
  enum {IR=2836};
  enum {NTAB=32};
  enum {NDIV=(1+(IM-1)/NTAB)};
  const double EPS;
  const double RNMX;
  const double AM;

  int idum;
  int iy;
  int* iv;
public: 
  Random(int seed);
  ~Random(void);
  double ran(void);
  void gaussian(double& deviate1, double& deviate2);
};

#endif
