"""The JAX model against nftsim, and its batching and gradients.

Run from prototypes/jax with ``python -m pytest tests``.  The comparisons
need the nftsim executable: $NFTSIM, else bin/nftsim at the repository root
(``make nftsim``).  Without it they are skipped, unless NFTSIM_REQUIRED=1
(as in CI), when they fail.
"""

import os

import numpy as np
import pytest
import jax
import jax.numpy as jnp

from nftjax import eirs, simulate, batch_simulate, stack_params, steady_state, nftsim_io
from nftjax.compare import CASES, case, compare

DT = eirs.DT
HAVE_NFTSIM = os.path.exists(nftsim_io.nftsim_path())
needs_nftsim = pytest.mark.skipif(
    not HAVE_NFTSIM and os.environ.get("NFTSIM_REQUIRED") != "1",
    reason="nftsim executable not found (make nftsim, or set NFTSIM)")


@needs_nftsim
@pytest.mark.parametrize("name", list(CASES))
def test_matches_nftsim(name):
    # 1024 steps (0.125 s) cover the pulse and the start of the response; the
    # tolerance is nf_numerical's (1e-9 of each trace's largest value), and
    # the agreement is ~1e-13 in practice.
    err, where, _ = compare(name, n_steps=1024)
    assert err < 1e-9, f"{name}: {where} differs by {err:.2e}"


@needs_nftsim
def test_output_interval_matches_nftsim():
    # Recording every 8 steps returns the same samples as nftsim's Interval.
    sp, p = case("grid4x4-rk4")
    out = simulate(sp, p, 512, out_every=8, record=lambda st: st["phi"][0])
    t, tr, _ = nftsim_io.run(eirs.to_conf(sp, p, 512 * DT, out_interval=8 * DT,
                                          outputs={"prop": "1"}))
    assert out.shape == tr["Propagator.1.phi"].shape
    np.testing.assert_allclose(np.asarray(out), tr["Propagator.1.phi"], rtol=1e-12)


def _pulse_batch(B):
    nus = np.array(eirs.NU) * (1 + 0.01 * np.linspace(-1, 1, B))[:, None]
    return [eirs.params(nu=nu, pulse_amp=10.0, pulse_width=32 * DT, pulse_onset=50 * DT)
            for nu in nus]


def test_batch_matches_single_runs():
    sp = eirs.spec(pulse=True)
    plist = _pulse_batch(3)
    rec = lambda st: st["phi"][0]
    batch = batch_simulate(sp, stack_params(plist), 512, 4, record=rec)
    for b, p in enumerate(plist):
        np.testing.assert_array_equal(np.asarray(batch[b]), np.asarray(simulate(sp, p, 512, 4,
                                                                                record=rec)))


def test_nodes_layout_matches_vmap():
    sp = eirs.spec(pulse=True, scheme="second", dendrite="integral")
    pb = stack_params(_pulse_batch(5))
    a = batch_simulate(sp, pb, 512, 4, layout="vmap")
    b = batch_simulate(sp, pb, 512, 4, layout="nodes")
    for k in a:
        np.testing.assert_allclose(np.asarray(a[k]), np.asarray(b[k]), rtol=1e-13, atol=0)


def test_steady_state_matches_config():
    sp = eirs.spec()
    q = np.asarray(steady_state(sp, eirs.params())["q0"][:4])
    np.testing.assert_allclose(q, eirs.Q0, rtol=1e-7)


def test_grad_matches_finite_differences():
    sp = eirs.spec(scheme="second", pulse=True, max_delay=0.05)
    pulse = dict(pulse_amp=10.0, pulse_width=32 * DT, pulse_onset=50 * DT)

    def trace(th):
        nu = jnp.asarray(eirs.NU).at[0].set(th[0]).at[1].set(th[1])
        p = steady_state(sp, eirs.params(nu=nu, t0=th[2], **pulse))
        return simulate(sp, p, 2048, 8, record=lambda st: st["phi"][0, 0], checkpoint=True)

    th0 = np.array([eirs.NU[0], eirs.NU[1], 0.0851])
    target = trace(th0 * np.array([1.01, 0.99, 1.0]))
    loss = jax.jit(lambda th: jnp.mean((trace(th) - target) ** 2))
    g = np.asarray(jax.grad(loss)(jnp.asarray(th0)))
    for i in range(3):
        h = 1e-6 * th0[i]
        tp, tm = th0.copy(), th0.copy()
        tp[i] += h
        tm[i] -= h
        fd = (float(loss(tp)) - float(loss(tm))) / (2 * h)
        assert abs(g[i] - fd) <= 1e-5 * abs(fd), (i, g[i], fd)
