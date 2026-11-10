8192 steps of Deltat = 2^-13 s (1 s)
| case | max relative difference, JAX vs nftsim | worst trace | nftsim vs nftsim (other FP flags) |
|---|---|---|---|
| node-rk4 | 3.4e-15 | Propagator.10.phi | 6.2e-15 |
| node-integral | 9.7e-14 | Propagator.2.phi | 3.0e-14 |
| node-rk4-second | 3.7e-15 | Propagator.10.phi | 5.2e-15 |
| node-integral-second | 8.9e-14 | Propagator.2.phi | 2.5e-14 |
| grid4x4-rk4 | 7.8e-14 | Propagator.1.phi | 4.7e-13 |
| grid8x8-integral | 6.7e-14 | Propagator.1.phi | 3.1e-13 |
| grid6x4-rk4-second | 7.5e-14 | Propagator.1.phi | 3.0e-13 |
| grid8x8-integral-second | 1.1e-13 | Propagator.2.phi | 3.4e-13 |
