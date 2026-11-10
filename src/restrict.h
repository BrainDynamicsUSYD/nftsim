/** @file restrict.h
  @brief NFTSIM_RESTRICT, a portable spelling of the `restrict` pointer qualifier.

  Qualifying a pointer with `restrict` promises the compiler that, while the
  pointer is in scope, the memory it points to is reached only through it (no
  other pointer aliases it). The compiler can then keep values in registers
  and vectorise loops that read through one pointer and write through another,
  as in the per-node loops of Dendrite, DendriteIntegral and StencilGrid.

  C99 has a `restrict` keyword, but no C++ standard (C++11 included) does, so
  the qualifier is only available as a compiler extension: `__restrict__` in
  GCC and Clang (and compilers that imitate GCC, eg Intel's), `__restrict` in
  MSVC. NFTSIM_RESTRICT expands to whichever the compiler supports, and to
  nothing for other compilers. The qualifier is only a promise that enables
  optimisation, so leaving it out never changes the results.

  Use it only for pointers that really are not aliased: if two restrict
  pointers do overlap, the behaviour is undefined.
*/

#ifndef NFTSIM_SRC_RESTRICT_H
#define NFTSIM_SRC_RESTRICT_H

#if defined(__GNUC__) || defined(__clang__)
#define NFTSIM_RESTRICT __restrict__
#elif defined(_MSC_VER)
#define NFTSIM_RESTRICT __restrict
#else
#define NFTSIM_RESTRICT
#endif

#endif //NFTSIM_SRC_RESTRICT_H
