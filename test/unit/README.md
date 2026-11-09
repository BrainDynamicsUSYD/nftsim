# Unit tests

This directory holds unit tests for individual nftsim classes.
They use a small harness in `unit_test.h` (plain functions, a `TEST` macro and `CHECK` macros), with no external dependencies.

Build and run them with:
```
make test-unit
```
or run the unit and numerical tests together with `make test`.
To run only the tests whose name contains a string, run the executable directly, eg `./bin/unit_tests stencil`.

## Writing a test

Add a `.cpp` file here (the Makefile compiles every `test/unit/*.cpp`) and include `unit_test.h` plus the nftsim headers you need:
```cpp
#include "unit_test.h"
#include "tau.h"

TEST(my_new_test) {
  CHECK(1 + 1 == 2);
  CHECK_EQ(2*3, 6);
  CHECK_CLOSE(0.1 + 0.2, 0.3, 1e-15);
  CHECK_MSG(true, "message shown on failure " << 42);
}
```
A failed check is reported with its file and line, and the test carries on, so one run shows every failure.

Most nftsim classes read their parameters from a `.conf` file through `Configf`.
`unit::TempConfig` writes a string to a temporary file for this, and deletes it afterwards:
```cpp
unit::TempConfig conf("Firing: Function: Sigmoid Theta: 0.01292 Sigma: 0.0038 Qmax: 340.0\n");
Configf configf(conf.path());
configf.param("Firing", firing_response);
```
Protected members can be reached from a small subclass defined in the test (see `test_firing_response.cpp` and `test_dendrite.cpp`).
Keep tests fast: the whole suite should run in well under a second.

## What is covered

| File | What it checks |
|---|---|
| `test_stencil.cpp` | `Stencil` and `StencilLegacy` neighbours and wrap-around on square and rectangular tori |
| `test_delays.cpp` | `Tau` rounding to whole steps; `Population::Q(tau)` lags, including a delay equal to the history length and per-node delays |
| `test_integrators.cpp` | `RK4` and `Euler` (`de.h`) errors and convergence order on a two-variable linear system with node-dependent rates |
| `test_dendrite.cpp` | `Dendrite` (RK4) and `DendriteIntegral` against the analytic step response; numeric initial `V` |
| `test_firing_response.cpp` | `FiringResponse` sigmoid and polynomial functions |
