"""Rough H100 throughput estimate from XLA's cost analysis of one step (an ESTIMATE).

Model: a step costs t = kernels * t_launch + bytes / bandwidth, where
kernels is the number of fused kernels XLA emits for one step, and bytes the
memory traffic of one step for the whole batch.  XLA's byte count treats the
in-place ring-buffer update as a full copy, so the buffer's contribution is
replaced by the one row written and the rows read per step.  float64
arithmetic (~1.1-1.4 kflop per node per step) is far below the H100's
~30 Tflop/s float64 peak and is ignored.

    python scripts/gpu_estimate.py
"""

import os
import sys
from dataclasses import replace

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import jax  # noqa: E402
import jax.numpy as jnp  # noqa: E402
from nftjax import eirs, make_step, init_state  # noqa: E402

DT = eirs.DT
BANDWIDTH = 2.5e12  # bytes/s: ~65% of the H100 NVL's 3.9 TB/s
LAUNCH = (2e-6, 5e-6)  # s per kernel: with CUDA graphs (XLA command buffers) .. without


def step_cost(g, B):
    """(kernels, bytes per step) for B parameter sets on a g x g grid (g=1: one node)."""
    out = []
    for delays in (True, False):
        sp = eirs.spec(ny=g, nx=g, noise=True, max_delay=eirs.T0 / 2 if delays else 0.0)
        if not delays:
            sp = replace(sp, connections=tuple(replace(c, delayed=False) for c in sp.connections))
        if g == 1:
            sp = replace(sp, nx=B, uncoupled=True)
            pb = {k: (jnp.moveaxis(v, 0, -1) if v.ndim > 1 else v)
                  for k, v in eirs.batch_params(B).items()}
            st = init_state(sp, pb, jax.random.PRNGKey(0))
            step = make_step(sp)
            c = jax.jit(lambda st, z: step(pb, st, 1, z)).lower(st, jnp.zeros((1, B))).compile()
        else:
            pb = eirs.batch_params(B)
            step = make_step(sp)
            st = jax.vmap(lambda p: init_state(sp, p, jax.random.PRNGKey(0)))(pb)
            f = jax.jit(jax.vmap(lambda p, s, z: step(p, s, 1, z)))
            c = f.lower(pb, st, jnp.zeros((B, 1, sp.nodes))).compile()
        ca = c.cost_analysis()
        ca = ca[0] if isinstance(ca, list) else ca
        out.append((c.as_text().count(" fusion("), ca["bytes accessed"]))
    (kern, _), (_, bytes_nodelay) = out
    nodes = g * g * B
    bytes_ring = nodes * 8 * (2 + 2 * 4)  # write 2 populations, read 4 connections x 2 rows
    return kern, bytes_nodelay + bytes_ring


def main():
    print("| model | batch | kernels/step | MB/step | est. step time (us) | est. throughput (sim-s/s) |")
    print("|---|---|---|---|---|---|")
    for g, batches in ((1, (1, 64, 4096, 32768, 262144)), (32, (1, 8, 64, 512, 4096))):
        # cost per node-step from a moderate batch, scaled to the others
        Bref = 4096 if g == 1 else 8
        kern, by = step_cost(g, Bref)
        per = by / (Bref * g * g)
        for B in batches:
            b = per * B * g * g
            t = [k_ * kern + b / BANDWIDTH for k_ in LAUNCH]
            thr = [B * DT / x for x in t]
            name = "single node" if g == 1 else f"{g}x{g} grid"
            print(f"| {name} | {B} | {kern} | {b / 1e6:.2f} | {t[0] * 1e6:.0f}-{t[1] * 1e6:.0f} | "
                  f"{thr[1]:.3g}-{thr[0]:.3g} |")


if __name__ == "__main__":
    main()
