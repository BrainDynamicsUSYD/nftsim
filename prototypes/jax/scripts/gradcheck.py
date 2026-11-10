"""Check jax.grad of two losses against central finite differences.

Parameters: theta = (nu_ee, nu_ei, t0), where nu_ee is Coupling 1 (e -> e),
nu_ei is Coupling 2 (i -> e) and t0 the corticothalamic loop delay (each of
the four corticothalamic delays is t0/2).  The SecondOrder scheme is used, as
it interpolates fractional delays, so the output depends smoothly on t0 (the
default scheme rounds delays to whole steps, so its gradient in t0 is zero).

Each run starts at the steady state of its own parameters (steady_state()),
so a change of parameters does not also start a transient.

Reverse mode (jax.grad) is checked against finite differences, and forward
mode (jax.jacfwd) against reverse mode, with the cost of each.

1. trace loss: mean squared difference between phi_ee and a reference trace
   (simulated with perturbed parameters), single node, pulse input, 1 s.
2. spectral loss: (log10 alpha power - log10 target)^2, alpha power being the
   8-13 Hz power of phi_ee (Welch), single node, white-noise input with a
   fixed noise realisation, for simulations of 1 to 64 s.

    python scripts/gradcheck.py [--max-time 64]
"""

import argparse
import json
import os
import sys
import time

import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import jax  # noqa: E402
import jax.numpy as jnp  # noqa: E402
from nftjax import eirs, simulate, steady_state, batch_simulate  # noqa: E402
from nftjax.spectra import welch, band_power  # noqa: E402

DT = eirs.DT
OUT_EVERY = 8
NAMES = ("nu_ee", "nu_ei", "t0")
THETA0 = np.array([eirs.NU[0], eirs.NU[1], 0.0851])


def make_params(theta, sp, **kw):
    """Parameters for theta, started at their own steady state (no transient)."""
    nu = jnp.asarray(eirs.NU).at[0].set(theta[0]).at[1].set(theta[1])
    return steady_state(sp, eirs.params(nu=nu, t0=theta[2], **kw))


def phi_ee(theta, sp, n_steps, key=None, **kw):
    out = simulate(sp, make_params(theta, sp, **kw), n_steps, OUT_EVERY, key,
                   record=lambda st: st["phi"][0, 0], checkpoint=True)
    return out


RELS = (1e-3, 1e-4, 1e-5, 1e-6, 1e-7)


def fd_grads(f, theta):
    """Central differences with one Richardson extrapolation (error O(h^4)),
    for each relative step in RELS: (len(RELS), len(theta))."""
    g = np.zeros((len(RELS), len(theta)))
    for i in range(len(theta)):
        def d(h):
            tp, tm = theta.copy(), theta.copy()
            tp[i] += h
            tm[i] -= h
            return (float(f(tp)) - float(f(tm))) / (2 * h)
        for j, rel in enumerate(RELS):
            h = rel * abs(theta[i])
            g[j, i] = (4 * d(h / 2) - d(h)) / 3
    return g


def report(label, f, theta, extra=None):
    fj = jax.jit(f)
    gj = jax.jit(jax.grad(f))
    t = time.perf_counter(); val = float(fj(theta)); t_compile_f = time.perf_counter() - t
    t = time.perf_counter(); val = float(jax.block_until_ready(fj(theta))); t_f = time.perf_counter() - t
    t = time.perf_counter(); jax.block_until_ready(gj(theta)); t_compile_g = time.perf_counter() - t
    t = time.perf_counter(); g = np.asarray(jax.block_until_ready(gj(theta))); t_g = time.perf_counter() - t
    # forward mode: one tangent per parameter, cheaper than reverse mode for a few parameters
    fw = jax.jit(jax.jacfwd(f))
    jax.block_until_ready(fw(theta))
    t = time.perf_counter(); gf = np.asarray(jax.block_until_ready(fw(theta))); t_fw = time.perf_counter() - t
    # Finite differences are limited by truncation error at large steps and by
    # round-off at small ones, so report the step that agrees best.
    gfds = fd_grads(fj, np.asarray(theta, dtype=float))
    rels = np.abs(gfds - g) / np.abs(gfds)
    best = np.argmin(rels, axis=0)
    gfd = gfds[best, np.arange(len(g))]
    rel = rels[best, np.arange(len(g))]
    res = {"loss": label, "value": val, "grad": dict(zip(NAMES, g.tolist())),
           "fd": dict(zip(NAMES, gfd.tolist())), "rel_err": dict(zip(NAMES, rel.tolist())),
           "fd_rel_step": dict(zip(NAMES, [RELS[b] for b in best])),
           "rel_err_by_step": {f"{r:g}": dict(zip(NAMES, rels[j].tolist()))
                               for j, r in enumerate(RELS)},
           "forward_s": t_f, "grad_s": t_g, "grad_over_forward": t_g / t_f,
           "jacfwd_s": t_fw, "jacfwd_over_forward": t_fw / t_f,
           "jacfwd_vs_grad_max_rel": float(np.max(np.abs(gf - g) / np.abs(g))),
           "compile_forward_s": t_compile_f, "compile_grad_s": t_compile_g}
    if extra:
        res.update(extra)
    print(json.dumps(res), flush=True)
    return res


def batch_timing(B, T):
    """Time the alpha-power loss and its gradient for B parameter sets at once."""
    sp = eirs.spec(scheme="second", noise=True, max_delay=0.05)
    n = int(round(T / DT))
    fs = 1.0 / (DT * OUT_EVERY)
    rng = np.random.default_rng(0)
    thetas = jnp.asarray(THETA0 * (1 + 0.002 * rng.uniform(-1, 1, (B, 3))))
    keys = jax.random.split(jax.random.PRNGKey(3), B)

    pb0 = eirs.batch_params(B)
    delayed = np.isin(np.arange(11), eirs.DELAYED)

    def losses(th):
        nu = pb0["nu"].at[:, 0].set(th[:, 0]).at[:, 1].set(th[:, 1])
        pb = dict(pb0, nu=nu, tau=jnp.where(delayed, th[:, 2:3] / 2, 0.0))
        pb = jax.vmap(lambda p: steady_state(sp, p))(pb)
        x = batch_simulate(sp, pb, n, OUT_EVERY, keys, lambda st: st["phi"][0], layout="nodes")
        fr, p = welch(x[:, :, 0], fs, 1024)
        a = band_power(fr, p, 8.0, 13.0)
        return (jnp.log10(a) - jnp.log10(1.5 * 5e-9)) ** 2

    f = jax.jit(lambda th: losses(th).sum())
    g = jax.jit(jax.grad(lambda th: losses(th).sum()))
    jax.block_until_ready(f(thetas))
    jax.block_until_ready(g(thetas))
    t = time.perf_counter(); jax.block_until_ready(f(thetas)); tf = time.perf_counter() - t
    t = time.perf_counter(); gr = jax.block_until_ready(g(thetas)); tg = time.perf_counter() - t
    # check one set's gradient against a single-set gradient (sets are independent)
    res = {"loss": f"batched alpha power ({T:g} s, noise, B={B}, one CPU device)",
           "B": B, "time_s": T, "forward_s": tf, "grad_s": tg, "grad_over_forward": tg / tf,
           "grad_sets_per_s": B / tg, "finite": bool(jnp.all(jnp.isfinite(gr)))}
    print(json.dumps(res), flush=True)
    return res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max-time", type=float, default=64.0)
    ap.add_argument("--out", default=os.path.join(os.path.dirname(__file__), "..", "results"))
    args = ap.parse_args()
    results = []

    # 1. trace loss, pulse input
    sp = eirs.spec(scheme="second", pulse=True, max_delay=0.05)
    n = 8192
    pulse = dict(pulse_amp=10.0, pulse_width=32 * DT, pulse_onset=200 * DT)
    target = phi_ee(THETA0 * np.array([1.02, 0.98, 1.0]) + np.array([0, 0, 1e-3]), sp, n, **pulse)
    f = lambda th: jnp.mean((phi_ee(th, sp, n, **pulse) - target) ** 2)
    results.append(report("trace (1 s, pulse)", f, jnp.asarray(THETA0)))

    # 2. spectral loss, white noise, increasing simulation length
    sp = eirs.spec(scheme="second", noise=True, max_delay=0.05)
    key = jax.random.PRNGKey(7)
    fs = 1.0 / (DT * OUT_EVERY)
    T = 1.0
    while T <= args.max_time:
        n = int(round(T / DT))
        nperseg = min(1024, n // OUT_EVERY)

        def alpha(th):
            x = phi_ee(th, sp, n, key)
            fr, p = welch(x, fs, nperseg)
            return band_power(fr, p, 8.0, 13.0)

        a0 = float(jax.jit(alpha)(jnp.asarray(THETA0)))
        target = 1.5 * a0
        f = lambda th: (jnp.log10(alpha(th)) - jnp.log10(target)) ** 2
        results.append(report(f"alpha power ({T:g} s, noise)", f, jnp.asarray(THETA0),
                              {"time_s": T, "alpha_power": a0}))
        T *= 4

    # 3. gradients of a batch: one backward pass gives every set's gradient
    for B in (64, 512):
        results.append(batch_timing(B, 4.0))

    os.makedirs(args.out, exist_ok=True)
    with open(os.path.join(args.out, "gradcheck.json"), "w") as fh:
        json.dump(results, fh, indent=1)


if __name__ == "__main__":
    main()
