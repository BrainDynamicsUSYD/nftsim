"""Deterministic comparison cases: the JAX model against nftsim on the same configuration."""

import numpy as np

from . import eirs, nftsim_io
from .model import simulate

DT = eirs.DT

# name: (ny, nx, scheme, dendrite, t0, length).  Grids need deltax <= Range/2
# (= 0.043 m), so their sheet is shorter than the 0.5 m of a 32 x 32 grid.
# t0 = 0.084 and 0.0851 s are not whole numbers of steps (t0/2 = 344.064 and
# 348.5 steps): rounded by the default scheme, interpolated by SecondOrder.
CASES = {
    "node-rk4": (1, 1, "default", "rk4", eirs.T0, 0.5),
    "node-integral": (1, 1, "default", "integral", eirs.T0, 0.5),
    "node-rk4-second": (1, 1, "second", "rk4", 0.0851, 0.5),
    "node-integral-second": (1, 1, "second", "integral", 0.0851, 0.5),
    "grid4x4-rk4": (4, 4, "default", "rk4", eirs.T0, 0.16),
    "grid8x8-integral": (8, 8, "default", "integral", eirs.T0, 0.32),
    "grid6x4-rk4-second": (6, 4, "second", "rk4", 0.084, 0.16),
    "grid8x8-integral-second": (8, 8, "second", "integral", 0.0851, 0.32),
}


def case(name):
    """Spec and parameters of a comparison case: a constant drive plus a
    rectangular pulse (at node 6 on a grid, everywhere on a single node)."""
    ny, nx, scheme, dendrite, t0, length = CASES[name]
    sp = eirs.spec(ny=ny, nx=nx, scheme=scheme, dendrite=dendrite, pulse=True,
                   pulse_nodes=(5,) if ny * nx > 1 else None, max_delay=t0 / 2)
    p = eirs.params(pulse_amp=10.0, pulse_width=32 * DT, pulse_onset=200 * DT,
                    t0=t0, length=length)
    return sp, p


def compare(name, n_steps=2048, exe=None, exe_ref=None):
    """Largest relative difference between the JAX model and nftsim over every trace.

    Each trace (phi of the 11 propagators and Q of the 5 populations, at every
    node and step) is compared as max|jax - nftsim| / max|nftsim|, and the
    largest is returned with its trace name.  With exe_ref, a second nftsim
    build is also compared with the first, which shows the size of round-off.
    """
    sp, p = case(name)
    out = simulate(sp, p, n_steps)
    conf = eirs.to_conf(sp, p, n_steps * DT)
    _, tr, _ = nftsim_io.run(conf, exe=exe)
    worst, where = 0.0, None
    pairs = [(f"Propagator.{c+1}.phi", out["phi"][:, c, :]) for c in range(sp.ncnt)]
    pairs += [(f"Pop.{a+1}.Q", out["Q"][:, a, :]) for a in range(sp.npop)]
    for key, a in pairs:
        b = tr[key]
        err = np.max(np.abs(np.asarray(a) - b)) / np.max(np.abs(b))
        if err > worst:
            worst, where = err, key
    ref = None
    if exe_ref:
        _, tr2, _ = nftsim_io.run(conf, exe=exe_ref)
        ref = max(np.max(np.abs(tr2[k] - tr[k])) / np.max(np.abs(tr[k])) for k, _ in pairs)
    return worst, where, ref
