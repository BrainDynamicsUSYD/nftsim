"""Throughput of the JAX model against nftsim, for batches of parameter sets.

Throughput is simulated seconds per wall-clock second, summed over the batch:
B * T / wall.  Every run is the noise-driven EIRS model (configs/
eirs-corticothalamic.conf values, with each nu perturbed by up to 0.5% per
parameter set) recording phi_ee at one node every 8 steps.

    python scripts/benchmark.py --model node --time 10
    python scripts/benchmark.py --model grid --time 0.25

JAX: on CPU the batch is split over --devices CPU devices (one thread each);
on a GPU it runs on the one GPU.  nftsim: B separate processes, at most
--procs at a time (default: one per core).  Compilation is timed separately
and excluded from the throughput.
"""

import argparse
import os
import sys

ap = argparse.ArgumentParser()
ap.add_argument("--model", choices=["node", "grid"], default="node")
ap.add_argument("--grid", type=int, default=32)
ap.add_argument("--time", type=float, default=None, help="simulated seconds per parameter set")
ap.add_argument("--batches", default="1,8,64,512,4096")
ap.add_argument("--devices", type=int, default=os.cpu_count(), help="CPU devices for JAX")
ap.add_argument("--procs", type=int, default=os.cpu_count(), help="concurrent nftsim processes")
ap.add_argument("--layout", default="auto", choices=["auto", "vmap", "nodes"])
ap.add_argument("--no-nftsim", action="store_true")
ap.add_argument("--no-jax", action="store_true")
ap.add_argument("--max-bytes", type=float, default=None,
                help="largest JAX batch state in bytes before splitting into chunks "
                     "(default: a third of physical memory)")
ap.add_argument("--per-device", type=int, default=None,
                help="largest number of sets per device per call; larger batches run as "
                     "consecutive chunks (default: no limit for one node, 8 for a grid on CPU, "
                     "whose state then stays in cache)")
ap.add_argument("--repeat", type=int, default=3)
ap.add_argument("--out", default=None)
args = ap.parse_args()

# Must precede the first import of jax.
os.environ.setdefault("XLA_FLAGS", f"--xla_force_host_platform_device_count={args.devices}")

import json  # noqa: E402
import platform  # noqa: E402
import subprocess  # noqa: E402
import tempfile  # noqa: E402
import time  # noqa: E402

import numpy as np  # noqa: E402

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import jax  # noqa: E402
from nftjax import eirs, nftsim_io  # noqa: E402
from nftjax.parallel import sharded_batch_simulate  # noqa: E402

DT = eirs.DT
OUT_EVERY = 8
g = args.grid if args.model == "grid" else 1
T = args.time if args.time is not None else (10.0 if g == 1 else 0.25)
n_steps = int(round(T / DT)) // OUT_EVERY * OUT_EVERY
T = n_steps * DT
length = 0.5
batches = [int(b) for b in args.batches.split(",")]
backend = jax.default_backend()
layout = args.layout
if layout == "auto":
    layout = "nodes" if g == 1 else "vmap"


def nu_batch(B, seed=0):
    rng = np.random.default_rng(seed)
    return np.array(eirs.NU) * (1.0 + 0.005 * rng.uniform(-1, 1, (B, 11)))


def physical_memory():
    try:
        return os.sysconf("SC_PAGE_SIZE") * os.sysconf("SC_PHYS_PAGES")
    except (ValueError, OSError):
        return 16e9


def state_bytes(sp):
    """Approximate bytes of one parameter set's simulation state."""
    N = sp.nodes
    return 8 * N * (2 * sp.history_length + 12 * sp.ncnt)


def bench_jax(B):
    sp = eirs.spec(ny=g, nx=g, noise=True)
    if backend == "cpu":
        devs = jax.devices()[: max(d for d in range(1, min(args.devices, B) + 1) if B % d == 0)]
    else:
        devs = jax.devices()[:1]
    max_bytes = args.max_bytes or (physical_memory() / 3 if backend == "cpu" else 60e9)
    per_device = args.per_device or (8 if g > 1 and backend == "cpu" else None)
    chunk = B if per_device is None else min(B, per_device * len(devs))
    while chunk * state_bytes(sp) > max_bytes and chunk % 2 == 0 and chunk > len(devs):
        chunk //= 2
    rec = (lambda st: st["phi"][0]) if layout == "nodes" else (lambda st: st["phi"][0, :1])
    f = jax.jit(lambda pb, keys: sharded_batch_simulate(sp, pb, n_steps, OUT_EVERY, keys, rec,
                                                         1, layout, devs))
    nus = nu_batch(B)
    keys = jax.random.split(jax.random.PRNGKey(1), B)
    parts = [(eirs.batch_params(chunk, nu=nus[i:i + chunk]), keys[i:i + chunk])
             for i in range(0, B, chunk)]

    def run_all():
        outs = [f(pb, kk) for pb, kk in parts]
        return [o.block_until_ready() for o in outs]

    t = time.perf_counter()
    out = run_all()
    first = time.perf_counter() - t
    times = []
    for _ in range(args.repeat if first < 30 else 1):
        t = time.perf_counter()
        out = run_all()
        times.append(time.perf_counter() - t)
    wall = min(times)
    finite = all(bool(np.all(np.isfinite(np.asarray(o)))) for o in out)
    return {"impl": "jax", "backend": backend, "device": str(devs[0].device_kind),
            "devices": len(devs), "layout": layout, "B": B, "chunk": chunk,
            "wall_s": wall, "first_call_s": first, "compile_s_est": first - wall,
            "throughput": B * T / wall, "finite": finite}


def bench_nftsim(B):
    exe = nftsim_io.nftsim_path()
    sp = eirs.spec(ny=g, nx=g, noise=True)
    d = tempfile.mkdtemp(prefix="nftbench-")
    confs = []
    for i, nu in enumerate(nu_batch(B)):
        path = os.path.join(d, f"r{i}.conf")
        with open(path, "w") as fh:
            fh.write(eirs.to_conf(sp, eirs.params(nu=nu, length=length), T,
                                  out_interval=OUT_EVERY * DT, out_nodes=[0],
                                  outputs={"prop": "1"}))
        confs.append(path)

    # Launched by xargs, as a shell sweep would be: spawning each process from
    # this (large, multithreaded) Python process costs about 0.1 s per run on macOS.
    procs = min(args.procs, B)
    listing = os.path.join(d, "confs.txt")
    with open(listing, "w") as fh:
        fh.write("\n".join(os.path.basename(c)[:-5] for c in confs) + "\n")
    cmd = f'xargs -P {procs} -I{{}} "{exe}" -i {{}}.conf -o {{}}.output < confs.txt'
    t = time.perf_counter()
    subprocess.run(cmd, shell=True, check=True, cwd=d, capture_output=True)
    wall = time.perf_counter() - t
    for path in confs:
        os.remove(path[:-5] + ".output")
    return {"impl": "nftsim", "procs": procs, "B": B, "wall_s": wall, "throughput": B * T / wall}


def loadavg():
    try:
        return os.getloadavg()
    except OSError:
        return None


results = {"model": f"{g}x{g}" if g > 1 else "single node", "time_s": T, "steps": n_steps,
           "machine": platform.platform(), "processor": platform.processor(),
           "cpu_count": os.cpu_count(), "jax": jax.__version__, "rows": []}
print(f"# {results['model']}, {T:g} s simulated per set ({n_steps} steps), backend {backend}",
      flush=True)
for B in batches:
    for which in ("jax", "nftsim"):
        if (which == "jax" and args.no_jax) or (which == "nftsim" and args.no_nftsim):
            continue
        la = loadavg()
        r = bench_jax(B) if which == "jax" else bench_nftsim(B)
        r["loadavg_before"] = la
        results["rows"].append(r)
        print(json.dumps(r), flush=True)

out = args.out or os.path.join(os.path.dirname(__file__), "..", "results",
                               f"bench_{args.model}_{backend}.json")
os.makedirs(os.path.dirname(out), exist_ok=True)
with open(out, "w") as fh:
    json.dump(results, fh, indent=1)
