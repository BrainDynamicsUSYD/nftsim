#ifndef RANDOM_H
#define RANDOM_H

class Random {
public: 
  Random(long seed);
  ~Random();
  double ran();
  void gaussian(double& deviate1, double& deviate2);
private:
  Random(Random& ); // no copy constructor
  long idum;
  enum {IA=16807}; // Use "enum hack" to obtain more portable static const int
  enum {IM=2147483647}; // Use "enum hack" to obtain more portable static const int
  enum {IQ=127773}; // Use "enum hack" to obtain more portable static const int
  enum {IR=2836}; // Use "enum hack" to obtain more portable static const int
  enum {NTAB=32}; // Use "enum hack" to obtain more portable static const int
  enum {NDIV=(1+(IM-1)/NTAB)}; // Use "enum hack" to obtain more portable static const int
  const double EPS;
  const double RNMX;
  const double AM;
//
  long iy;
  long* iv;
};

#endif
