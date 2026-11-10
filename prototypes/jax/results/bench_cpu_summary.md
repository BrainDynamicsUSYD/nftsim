
single node, 10 s simulated per parameter set, throughput in simulated s per wall s (bench_node_cpu.json)

| batch | JAX cpu (cpu, up to 8 devices, nodes layout) | JAX compile (s, approx.) | nftsim (up to 10 processes) | JAX / nftsim |
|---|---|---|---|---|
| 1 | 750 | 0.27 | 84.3 | 8.9 |
| 8 | 4,194 | 0.43 | 364 | 12 |
| 64 | 6,679 | 0.97 | 443 | 15 |
| 512 | 8,184 | 0.75 | 427 | 19 |
| 4096 | 6,661 | 1.3 | 427 | 16 |

32x32, 0.25 s simulated per parameter set, throughput in simulated s per wall s (bench_grid_cpu.json)

| batch | JAX cpu (cpu, up to 8 devices, vmap layout) | JAX compile (s, approx.) | nftsim (up to 10 processes) | JAX / nftsim |
|---|---|---|---|---|
| 1 | 0.921 | 0.76 | 1.27 | 0.73 |
| 8 | 7.12 | 0.94 | 9.98 | 0.71 |
| 64 | 5.82 | 1.7 | 9.75 | 0.6 |
| 512 | 6.05 (8 chunks) | - | 10.7 | 0.57 |
| 4096 | 5.84 (64 chunks) | - | 10.4 | 0.56 |
