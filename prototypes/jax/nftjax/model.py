"""Neural field model of nftsim, written in JAX.

The model is a set of populations joined by connections.
Each connection c from population b to population a has a propagator, which
turns the presynaptic firing rate Q_b (delayed by tau_c) into an axonal field
phi_c, a coupling P_c = nu_c phi_c, and a dendrite, which filters P_c into the
postsynaptic potential V_c:

    D_c V_c = nu_c phi_c,  D = 1/(alpha beta) d2/dt2 + (1/alpha + 1/beta) d/dt + 1.

The soma potential is V_a = sum_c V_c over the connections into a, and the
firing rate is the sigmoid Q_a = Qmax / (1 + exp(-(V_a - theta)/sigma)).
Stimulus populations fire a prescribed time series instead (a constant mean,
plus optional white noise and a rectangular pulse).

Propagators:
  * ``map``: phi = Q_b(t - tau).
  * ``harmonic``: (1/gamma^2 d2/dt2 + 2/gamma d/dt + 1) phi = Q_b(t - tau),
    integrated by RK4 with Q held constant over the step (nftsim's Harmonic).
  * ``wave``: the damped wave equation with -r^2 laplacian on a periodic grid,
    nftsim's explicit five-point scheme (src/wave.cpp).  On a single node it
    is the harmonic propagator, as in nftsim.

One time-step follows src/solver.cpp: couplings (P = nu phi with the phi of
the previous step), then populations (dendrites, soma, firing rate), then
propagators (with the firing rates just computed).  Every floating-point
expression is written in the same order as in the C++ code, so that results
agree with nftsim to round-off.

Two time-stepping schemes are supported, as in nftsim (README.md, "Time-stepping
scheme"): ``default`` (first order; delays rounded to whole steps) and
``second`` (``Integration: SecondOrder``: dendrite inputs extrapolated
linearly over a step, Harmonic Q linear over a step, fractional delays
interpolated).  The second scheme makes the output differentiable with
respect to the delays.

Everything is a pure function of a parameter pytree, so ``jax.vmap`` runs a
batch of parameter sets and ``jax.grad`` differentiates through the run.
"""

from dataclasses import dataclass, replace
import math

import numpy as np
import jax
import jax.numpy as jnp
from jax import lax

jax.config.update("jax_enable_x64", True)

TIME_TOLERANCE = 1.0e-6  # src/timeseries.h, in units of Deltat


@dataclass(frozen=True)
class Connection:
    """A connection pre -> post (0-based population indices)."""
    pre: int
    post: int
    propagator: str = "map"     # "map", "wave" or "harmonic"
    dendrite: str = "rk4"       # "rk4" (nftsim's default Dendrite) or "integral"
    delayed: bool = False       # whether tau may be non-zero (needs a ring buffer)


@dataclass(frozen=True)
class Spec:
    """Static structure of a simulation (anything that changes array shapes or code).

    Values that may vary between parameter sets, or that are differentiated,
    are in the parameter pytree instead (see ``param_names``).
    """
    neural: tuple               # one bool per population (False: stimulus)
    connections: tuple          # Connection objects, in nftsim's order
    dt: float                   # Deltat
    ny: int = 1                 # grid rows (shortside)
    nx: int = 1                 # grid columns (longside); ny*nx = Nodes
    scheme: str = "default"     # "default" or "second"
    max_delay: float = 0.0      # largest tau in any parameter set (sizes the ring buffer)
    noise: bool = False         # white noise on the stimulus populations
    pulse: bool = False         # a rectangular pulse on the stimulus populations
    pulse_nodes: tuple = None   # 0-based nodes the pulse is applied to (None: all)
    uncoupled: bool = False     # nodes are independent (wave -> harmonic): a batch laid out as nodes

    @property
    def nodes(self):
        return self.ny * self.nx

    @property
    def npop(self):
        return len(self.neural)

    @property
    def ncnt(self):
        return len(self.connections)

    @property
    def history_length(self):
        # m+1 points for a delay of m steps, one more for interpolation, one spare.
        return int(math.ceil(self.max_delay / self.dt - 1e-9)) + 3


# Per-connection, per-population and global parameters.  All are float64
# arrays; per-connection entries that a connection does not use are ignored.
PARAM_SHAPES = {
    "nu": "c", "alpha": "c", "beta": "c", "tau": "c", "gamma": "c", "range": "c",
    "theta": "p", "sigma": "p", "qmax": "p", "q0": "p",
    "stim_mean": "p", "stim_asd": "p",
    "pulse_amp": "p", "pulse_width": "p", "pulse_onset": "p",
    "pulse_period": "p", "pulse_count": "p",
    "length": "",
}


def _cnt_groups(spec):
    cs = spec.connections
    single = spec.nodes == 1
    kind = []
    for c in cs:
        k = c.propagator
        if k == "wave" and (single or spec.uncoupled):
            k = "harmonic"  # as nftsim does for Nodes: 1
        if k not in ("map", "wave", "harmonic"):
            raise ValueError(f"unknown propagator {c.propagator!r}")
        kind.append(k)
    groups = {k: np.array([i for i, kk in enumerate(kind) if kk == k], dtype=int)
              for k in ("map", "wave", "harmonic")}
    dgroups = {k: np.array([i for i, c in enumerate(cs) if c.dendrite == k], dtype=int)
               for k in ("rk4", "integral")}
    if sum(len(v) for v in dgroups.values()) != len(cs):
        raise ValueError("dendrite must be 'rk4' or 'integral'")
    delayed_pops = sorted({c.pre for c in cs if c.delayed})
    return groups, dgroups, delayed_pops


def _col(x):
    """A per-connection or per-population parameter as a column: (n,) -> (n, 1).

    Parameters may also be given per node, with shape (n, nodes), as nftsim
    allows for nu and tau; those are returned unchanged.
    """
    return x[:, None] if x.ndim == 1 else x


def _sel(x, idx):
    """x[idx] for a static list of indices, without a gather."""
    return jnp.stack([x[j] for j in idx])


def _read(h, idx):
    """Row idx of a history h (L, nodes); idx is a scalar or one index per node."""
    if jnp.ndim(idx) == 0:
        return lax.dynamic_index_in_dim(h, idx, axis=0, keepdims=False)
    return jnp.take_along_axis(h, idx[None, :], axis=0)[0]


def _qinit(spec, p):
    """Initial firing rate of each population: Q for neural, Mean for stimulus."""
    neural = np.array(spec.neural)
    if jnp.ndim(p["q0"]) == 2:
        neural = neural[:, None]
    return jnp.where(neural, p["q0"], p["stim_mean"])


def _delay_steps(spec, tau):
    """Whole steps m and fraction f of each delay, tau = (m + f) dt (src/tau.cpp)."""
    s = tau / spec.dt
    r = jnp.floor(s + 0.5)  # lround, for tau >= 0
    if spec.scheme == "default":
        return r.astype(jnp.int32), jnp.zeros_like(s)
    whole = jnp.abs(s - r) <= 1e-6  # is_step_multiple
    fl = jnp.floor(s)
    m = jnp.where(whole, r, fl)
    f = jnp.where(whole, 0.0, s - lax.stop_gradient(fl))
    return m.astype(jnp.int32), f


def init_state(spec, p, key=None):
    """State at t = 0: every population at its initial Q, every field at rest."""
    groups, dgroups, dpops = _cnt_groups(spec)
    N, C = spec.nodes, spec.ncnt
    pre = np.array([c.pre for c in spec.connections])
    q0 = _col(_qinit(spec, p))  # (P, 1) or (P, N)
    phi0 = jnp.broadcast_to(q0[pre], (C, N))
    v0 = _col(p["nu"]) * phi0
    st = {
        "phi": phi0,
        "v": v0,
        "dv": jnp.zeros((C, N)),
        "pnp": v0,  # previous nu*phi (Integral dendrites; SecondOrder)
    }
    h = groups["harmonic"]
    if len(h):
        st["h_dphi"] = jnp.zeros((len(h), N))
        st["h_oldq"] = jnp.broadcast_to(q0[pre[h]], (len(h), N))
    w = groups["wave"]
    if len(w):
        st["w_oldp"] = phi0[w]
        st["w_q"] = jnp.broadcast_to(q0[pre[w]], (len(w), N))
        st["w_oldq"] = st["w_q"]
    if dpops:
        L = spec.history_length
        st["hist"] = jnp.broadcast_to(q0[np.array(dpops)][:, None, :], (len(dpops), L, N))
    if spec.noise:
        st["key"] = key if key is not None else jax.random.PRNGKey(0)
    st["Q"] = jnp.broadcast_to(q0, (spec.npop, N))  # recorded only after a step
    return st


def _sigmoid(v, theta, sigma, qmax):
    return qmax / (1.0 + jnp.exp(-(v - theta) / sigma))


def _dendrite_rk4(spec, np_, pp, v0, w0, alpha, beta):
    """src/dendrite.cpp: RK4 with nu*phi constant (default) or extrapolated (second)."""
    h = spec.dt
    h2 = spec.dt * 0.5
    h6 = spec.dt / 6.0
    fab = _col(1.0 / alpha + 1.0 / beta)
    ab = _col(alpha * beta)
    if spec.scheme == "second":
        p0 = np_
        dp = p0 - pp
        pa = p0
        pm = p0 + 0.5 * dp
        p1 = p0 + dp
    else:
        pa = pm = p1 = np_
    kv1 = w0
    kw1 = (pa - v0 - fab * w0) * ab
    v1 = v0 + (h2 * kv1)
    w1 = w0 + (h2 * kw1)
    kv2 = w1
    kw2 = (pm - v1 - fab * w1) * ab
    v2 = v0 + (h2 * kv2)
    w2 = w0 + (h2 * kw2)
    kv3 = w2
    kw3 = (pm - v2 - fab * w2) * ab
    v3 = v0 + (h * kv3)
    w3 = w0 + (h * kw3)
    kv4 = w3
    kw4 = (p1 - v3 - fab * w3) * ab
    v = v0 + h6 * (kv1 + (2.0 * kv2) + (2.0 * kv3) + kv4)
    w = w0 + h6 * (kw1 + (2.0 * kw2) + (2.0 * kw3) + kw4)
    return v, w


def _dendrite_integral(spec, np_, onp, vv, dv, alpha, beta):
    """src/dendrite_integral.cpp, alpha != beta: exact solution for linear nu*phi."""
    dt = spec.dt
    fab = _col(1.0 / alpha + 1.0 / beta)
    a = _col(alpha)
    b = _col(beta)
    expa = jnp.exp(-a * dt)
    expb = jnp.exp(-b * dt)
    inv_amb = 1.0 / (a - b)
    invdt = 1.0 / dt
    if spec.scheme == "second":
        p0 = np_
        dp = p0 - onp
        p1 = p0 + dp
        dpdt = dp * invdt
        start = p0
        end = p1
    else:
        dpdt = (np_ - onp) * invdt
        start = onp
        end = np_
    adj = start - fab * dpdt - vv
    C1 = (adj * b - dv + dpdt) * inv_amb
    C1expa = C1 * expa
    C2expb = expb * (-C1 - adj)
    v = C1expa + C2expb + end - fab * dpdt
    w = C1expa * (-a) + C2expb * (-b) + dpdt
    return v, w


def _harmonic(spec, q, oldq, phi, dphi, gamma):
    """src/harmonic.cpp with the RK4 of src/de.h."""
    dt = spec.dt
    d5 = dt * 0.5
    h6 = dt / 6.0
    g2 = _col(gamma * gamma)
    tog = _col(2.0 / gamma)
    if spec.scheme == "second":
        dqdt = (q - oldq) / dt
        q1 = oldq
        q2 = oldq + (d5 * dqdt)
        q4 = oldq + (dt * dqdt)
        newoldq = q
    else:
        q1 = q2 = q4 = q
        newoldq = oldq

    def rhs(y0, y1, yq):
        return y1, g2 * (yq - (tog * y1) - y0)

    k10, k11 = rhs(phi, dphi, q1)
    k20, k21 = rhs(phi + (d5 * k10), dphi + (d5 * k11), q2)
    k30, k31 = rhs(phi + (d5 * k20), dphi + (d5 * k21), q2)
    k40, k41 = rhs(phi + (dt * k30), dphi + (dt * k31), q4)
    phi = phi + h6 * (k10 + (2.0 * k20) + (2.0 * k30) + k40)
    dphi = dphi + h6 * (k11 + (2.0 * k21) + (2.0 * k31) + k41)
    return phi, dphi, newoldq


def _wave(spec, qnew, q, oldq, p, oldp, gamma, rng, length):
    """src/wave.cpp: explicit five-point scheme on a periodic ny x nx grid."""
    dt = spec.dt
    nw = p.shape[0]
    shape = (nw, spec.ny, spec.nx)
    g = _col(gamma)
    r = _col(rng)
    deltax = length / spec.nx
    dt2on12 = dt * dt / 12.0
    dfact = dt2on12 * g * g
    dt2ondx2 = (dt * dt) / (deltax * deltax)
    p2 = dt2ondx2 * r * r * g * g
    tenminus4p2 = 10.0 - 4.0 * p2
    twominus4p2 = 2.0 - 4.0 * p2
    expfactneg = jnp.exp(-dt * g)
    expfactpos = jnp.exp(dt * g)

    def nsum(f):
        f = f.reshape(shape)
        n = jnp.roll(f, 1, axis=1)   # row above (ptr - longside - 2)
        s = jnp.roll(f, -1, axis=1)  # row below
        w = jnp.roll(f, 1, axis=2)   # left
        e = jnp.roll(f, -1, axis=2)  # right
        return (n + s + w + e).reshape(nw, -1)

    sump = nsum(p)
    sumq = nsum(q)
    drive = dfact * (tenminus4p2 * q + qnew * expfactpos + oldq * expfactneg + p2 * sumq)
    pnew = expfactneg * (twominus4p2 * p + p2 * sump - oldp * expfactneg + drive)
    # new state: phi^n = pnew, phi^{n-1} = p, Q^n = qnew, Q^{n-1} = q
    return pnew, p, qnew, q


def _stimulus(spec, p, k, z, idx):
    """Firing rates of the stimulus populations idx at step k (src/timeseries.cpp)."""
    dt = spec.dt
    p = {key: _sel(p[key], idx) for key in
         ("stim_mean", "stim_asd", "pulse_amp", "pulse_width", "pulse_onset",
          "pulse_period", "pulse_count")} | {"length": p["length"]}
    q = _col(p["stim_mean"]) * jnp.ones((1, spec.nodes))
    if spec.noise:
        if spec.nodes > 1:
            dx = p["length"] / spec.nx
            sd = jnp.sqrt(2.0 * 4.0 * math.pi ** 3 * p["stim_asd"] ** 2 / dt / dx ** 2)
        else:
            sd = jnp.sqrt(2.0 * math.pi * p["stim_asd"] ** 2 / dt)
        q = q + _col(sd) * z
    if spec.pulse:
        t = -p["pulse_onset"] + k * dt
        t = jnp.where(jnp.abs(t) < TIME_TOLERANCE * dt, 0.0, t)
        ttol = t + TIME_TOLERANCE * dt
        period = p["pulse_period"]
        on = (t >= 0.0) & (jnp.fmod(ttol, period) < p["pulse_width"]) \
            & (ttol / period < p["pulse_count"])
        amp = _col(jnp.where(on, p["pulse_amp"], 0.0))
        if spec.pulse_nodes is not None:
            mask = np.zeros(spec.nodes)
            mask[list(spec.pulse_nodes)] = 1.0
            q = jnp.where(mask[None, :] > 0, q + amp, q)
        else:
            q = q + amp
    return q


def make_step(spec):
    """Return step(p, state, k, z) -> state, advancing from step k-1 to step k.

    z holds the standard normal variates of this step, (stimulus populations,
    nodes), when spec.noise is set (otherwise None).
    """
    groups, dgroups, dpops = _cnt_groups(spec)
    cs = spec.connections
    C, P, N = spec.ncnt, spec.npop, spec.nodes
    dslot = {pop: i for i, pop in enumerate(dpops)}
    neural_idx = [a for a in range(P) if spec.neural[a]]
    stim_idx = [a for a in range(P) if not spec.neural[a]]
    into = {a: [i for i, c in enumerate(cs) if c.post == a] for a in range(P)}
    L = spec.history_length
    mixed = len(dgroups["rk4"]) and len(dgroups["integral"])
    is_rk4 = np.zeros((C, 1), dtype=bool)
    is_rk4[dgroups["rk4"]] = True

    def rows(x, idx):
        # x[idx] for a static index list, as slices (a gather is slow on CPU)
        return jnp.stack([x[j] for j in idx])

    def step(p, st, k, z=None):
        st = dict(st)
        # 1. couplings: P = nu * phi (phi from the previous step)
        P_ = _col(p["nu"]) * st["phi"]
        # 2. populations: dendrites, soma, firing
        args = (P_, st["pnp"], st["v"], st["dv"], p["alpha"], p["beta"])
        if mixed:  # both kinds on every connection, then select: cheaper than scatter
            v1, w1 = _dendrite_rk4(spec, *args)
            v2, w2 = _dendrite_integral(spec, *args)
            v, dv = jnp.where(is_rk4, v1, v2), jnp.where(is_rk4, w1, w2)
        elif len(dgroups["rk4"]):
            v, dv = _dendrite_rk4(spec, *args)
        else:
            v, dv = _dendrite_integral(spec, *args)
        st["v"], st["dv"], st["pnp"] = v, dv, P_
        qs = [None] * P
        for a in neural_idx:
            idx = into[a]
            va = v[idx[0]]
            for j in idx[1:]:
                va = va + v[j]  # summed in connection order, as in nftsim
            qs[a] = _sigmoid(va, p["theta"][a], p["sigma"][a], p["qmax"][a])
        if stim_idx:
            qstim = _stimulus(spec, p, k, z, stim_idx)
            for n, a in enumerate(stim_idx):
                qs[a] = qstim[n]
        for a in range(P):
            if qs[a] is None:  # a neural population without inputs
                qs[a] = jnp.zeros(N)
        Q = jnp.stack(qs)
        # ring buffer: write the new Q of every delayed population at slot k % L
        if dpops:
            st["hist"] = lax.dynamic_update_slice(
                st["hist"], rows(Q, dpops)[:, None, :], (0, k % L, 0))
            m, f = _delay_steps(spec, p["tau"])
        # 3. propagators, given the (delayed) Q of every connection
        qin = []
        for ci, c in enumerate(cs):
            if c.delayed:
                h = st["hist"][dslot[c.pre]]
                a_ = _read(h, (k - m[ci]) % L)
                if spec.scheme == "second":
                    b_ = _read(h, (k - m[ci] - 1) % L)
                    a_ = a_ + f[ci] * (b_ - a_)
                qin.append(a_)
            else:
                qin.append(Q[c.pre])
        phi = list(qin)  # map propagators: phi = Q(t - tau)
        i = list(groups["harmonic"])
        if i:
            ph, dph, oq = _harmonic(spec, jnp.stack([qin[j] for j in i]), st["h_oldq"],
                                    rows(st["phi"], i), st["h_dphi"], _sel(p["gamma"], i))
            for n, j in enumerate(i):
                phi[j] = ph[n]
            st["h_dphi"], st["h_oldq"] = dph, oq
        i = list(groups["wave"])
        if i:
            pn, po, qn, qo = _wave(spec, jnp.stack([qin[j] for j in i]), st["w_q"],
                                   st["w_oldq"], rows(st["phi"], i), st["w_oldp"],
                                   _sel(p["gamma"], i), _sel(p["range"], i), p["length"])
            for n, j in enumerate(i):
                phi[j] = pn[n]
            st["w_oldp"], st["w_q"], st["w_oldq"] = po, qn, qo
        st["phi"] = jnp.stack(phi)
        st["Q"] = Q
        return st

    return step


def default_record(st):
    """Record phi of every connection and Q of every population at every node."""
    return {"phi": st["phi"], "Q": st["Q"]}


def simulate(spec, p, n_steps, out_every=1, key=None, record=default_record,
             unroll=1, checkpoint=False):
    """Run n_steps steps and return record(state) after every out_every steps.

    Outputs are at steps out_every, 2*out_every, ..., as nftsim writes them for
    ``Output: Start: 0 Interval: out_every*Deltat``.  The leading axis of every
    recorded array is time (n_steps // out_every entries).
    ``checkpoint=True`` rematerialises each output block in the backward pass,
    so reverse-mode memory scales with the number of outputs, not of steps.
    """
    if n_steps % out_every:
        raise ValueError("n_steps must be a multiple of out_every")
    step = make_step(spec)
    st = init_state(spec, p, key)

    nstim = sum(1 for a in spec.neural if not a)

    def block(st, j):
        def inner(s, x):
            i, z = x
            return step(p, s, j * out_every + i + 1, z), None
        z = None
        if spec.noise:
            # The variates of a whole block at once: drawing them step by step
            # inside the loop is an order of magnitude slower on CPU.
            z = jax.random.normal(jax.random.fold_in(st["key"], j),
                                  (out_every, nstim, spec.nodes))
            # Stops XLA fusing the generator into each step's use of z, which
            # recomputes the whole block of variates every step.
            z = lax.optimization_barrier(z)
        st, _ = lax.scan(inner, st, (jnp.arange(out_every), z), unroll=unroll)
        return st

    if checkpoint:
        block = jax.checkpoint(block)

    def outer(st, j):
        st = block(st, j)
        return st, record(st)

    _, out = lax.scan(outer, st, jnp.arange(n_steps // out_every))
    return out


def stack_params(plist):
    """Stack a list of parameter dicts into one batched dict (for vmap)."""
    return jax.tree_util.tree_map(lambda *xs: jnp.stack(xs), *plist)


def batch_simulate(spec, pbatch, n_steps, out_every=1, keys=None, record=default_record,
                   unroll=1, layout="vmap"):
    """Simulate a batch of parameter sets (the leading axis of every parameter).

    layout="vmap" maps simulate() over the batch with jax.vmap, so arrays are
    (batch, ..., nodes).  layout="nodes" (single-node specs only) instead runs
    the batch as the nodes of one uncoupled simulation with per-node
    parameters, so the batch is the minor (contiguous) axis, which XLA's CPU
    backend vectorises far better when there is one node.  The two layouts give
    the same numbers for deterministic runs; with noise the random streams differ.
    Either way the result has a leading batch axis.  With layout="nodes",
    record must keep the node axis last (as default_record does).
    """
    B = jax.tree_util.tree_leaves(pbatch)[0].shape[0]
    if layout == "nodes":
        if spec.nodes != 1:
            raise ValueError("layout='nodes' needs a single-node spec")
        sp = replace(spec, ny=1, nx=B, uncoupled=True)
        # (B, n) -> (n, B): one value per node; global scalars (B,) stay per node
        pn = {k: (jnp.moveaxis(v, 0, -1) if v.ndim > 1 else v) for k, v in pbatch.items()}
        key = keys[0] if keys is not None else jax.random.PRNGKey(0)
        out = simulate(sp, pn, n_steps, out_every, key, record, unroll)
        # (time, ..., B) -> (B, time, ..., 1)
        return jax.tree_util.tree_map(lambda x: jnp.moveaxis(x, -1, 0)[..., None], out)
    if spec.noise:
        if keys is None:
            keys = jax.random.split(jax.random.PRNGKey(0), B)
        f = lambda pp, kk: simulate(spec, pp, n_steps, out_every, kk, record, unroll)
        return jax.vmap(f)(pbatch, keys)
    f = lambda pp: simulate(spec, pp, n_steps, out_every, None, record, unroll)
    return jax.vmap(f)(pbatch)


def steady_state(spec, p, iters=40):
    """Steady firing rates with p["q0"] as the initial guess: p with q0 replaced.

    At a steady state every phi equals the presynaptic Q and every dendrite
    potential is nu*phi, so Q_a = S_a(sum over inputs c of nu_c Q_pre(c)),
    with the stimulus populations at their means.  Solved by Newton's method
    (a fixed number of iterations, so it is differentiable: its gradient is
    that of the implicit function).  Starting a run from the steady state of
    its own parameters removes the initial transient that a fixed q0 causes
    when parameters change, which otherwise dominates short-run gradients.
    Assumes one value per population (not per node) and that the guess is in
    the basin of the wanted fixed point.
    """
    cs = spec.connections
    neural = np.array(spec.neural)
    pre = np.array([c.pre for c in cs])
    post = np.array([c.post for c in cs])
    M = np.zeros((spec.npop, spec.ncnt))
    M[post, np.arange(spec.ncnt)] = 1.0  # sums the connections into each population
    stim = jnp.where(neural, 0.0, p["stim_mean"])

    def resid(q):
        q = jnp.where(neural, q, stim)
        v = M @ (p["nu"] * q[pre])
        s = _sigmoid(v, p["theta"], jnp.where(neural, p["sigma"], 1.0), p["qmax"])
        return jnp.where(neural, s - q, 0.0)

    def newton(q, _):
        J = jax.jacfwd(resid)(q) + jnp.diag(jnp.where(neural, 0.0, 1.0))
        return q - jnp.linalg.solve(J, resid(q)), None

    q, _ = lax.scan(newton, jnp.where(neural, p["q0"], 0.0), None, length=iters)
    return dict(p, q0=jnp.where(neural, q, 0.0))
