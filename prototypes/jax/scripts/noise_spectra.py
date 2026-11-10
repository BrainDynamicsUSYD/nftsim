"""Compare power spectra of noise-driven runs of the JAX model and nftsim.

The random streams differ, so the comparison is statistical: R runs of each
(nftsim with Ranseed 1..R, JAX with R independent keys), and for each
frequency bin the mean over runs of log10 PSD, with its standard error.

    python scripts/noise_spectra.py [--runs 16] [--time 32] [--grid 16]
"""

import argparse
import concurrent.futures as cf
import json
import os
import sys

import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import jax  # noqa: E402
from nftjax import eirs, nftsim_io, batch_simulate, stack_params  # noqa: E402
from nftjax.spectra import welch  # noqa: E402

DT = eirs.DT
OUT_EVERY = 8  # 1024 Hz sampling
FS = 1.0 / (DT * OUT_EVERY)
NPERSEG = 2048  # 0.5 Hz resolution


def nftsim_runs(sp, p, T, runs):
    def one(seed):
        conf = eirs.to_conf(sp, p, T, out_interval=OUT_EVERY * DT,
                            outputs={"prop": "1"}, seed=seed)
        _, tr, _ = nftsim_io.run(conf)
        return tr["Propagator.1.phi"].T  # (nodes, time)
    with cf.ThreadPoolExecutor(max_workers=8) as ex:
        return np.stack(list(ex.map(one, range(1, runs + 1))))  # (runs, nodes, time)


def jax_runs(sp, p, T, runs):
    n = int(round(T / DT))
    keys = jax.random.split(jax.random.PRNGKey(2026), runs)
    out = batch_simulate(sp, stack_params([p] * runs), n, OUT_EVERY, keys,
                         record=lambda st: st["phi"][0])
    return np.moveaxis(np.asarray(out), -1, 1)  # (runs, nodes, time)


def compare(label, sp, p, T, runs):
    a = nftsim_runs(sp, p, T, runs)
    b = jax_runs(sp, p, T, runs)
    f, pa = welch(a, FS, NPERSEG)
    _, pb = welch(b, FS, NPERSEG)
    f = np.asarray(f)
    la = np.log10(np.asarray(pa).mean(axis=1))  # (runs, freq), averaged over nodes
    lb = np.log10(np.asarray(pb).mean(axis=1))
    sel = (f >= 1.0) & (f <= 100.0)
    ma, mb = la.mean(0)[sel], lb.mean(0)[sel]
    se = np.sqrt(la.var(0, ddof=1)[sel] / runs + lb.var(0, ddof=1)[sel] / runs)
    z = (mb - ma) / se
    fs = f[sel]
    alpha = (fs >= 8) & (fs <= 13)
    res = {
        "case": label, "runs": runs, "time_s": T,
        "mean_ratio_jax_over_nftsim_1_100Hz": float(10 ** np.mean(mb - ma)),
        "rms_log10_ratio": float(np.sqrt(np.mean((mb - ma) ** 2))),
        "alpha_power_ratio": float(np.sum(10 ** mb[alpha]) / np.sum(10 ** ma[alpha])),
        "max_abs_z": float(np.max(np.abs(z))),
        "frac_abs_z_gt_2": float(np.mean(np.abs(z) > 2)),
        "frac_abs_z_gt_3": float(np.mean(np.abs(z) > 3)),
        "peak_freq_nftsim": float(fs[np.argmax(ma[(fs > 5)]) + np.sum(fs <= 5)]),
        "peak_freq_jax": float(fs[np.argmax(mb[(fs > 5)]) + np.sum(fs <= 5)]),
        "n_bins": int(sel.sum()),
    }
    print(json.dumps(res, indent=1), flush=True)
    return res, (fs, ma, mb, se)


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--runs", type=int, default=16)
    ap.add_argument("--time", type=float, default=32.0)
    ap.add_argument("--grid", type=int, default=16)
    ap.add_argument("--grid-time", type=float, default=8.0)
    ap.add_argument("--grid-runs", type=int, default=8)
    ap.add_argument("--out", default=os.path.join(os.path.dirname(__file__), "..", "results"))
    args = ap.parse_args()
    results = []
    sp = eirs.spec(noise=True)
    r, curves1 = compare("single node", sp, eirs.params(), args.time, args.runs)
    results.append(r)
    g = args.grid
    sp = eirs.spec(ny=g, nx=g, noise=True)
    r, curves2 = compare(f"{g}x{g} grid", sp, eirs.params(length=0.5), args.grid_time,
                         args.grid_runs)
    results.append(r)
    os.makedirs(args.out, exist_ok=True)
    with open(os.path.join(args.out, "noise_spectra.json"), "w") as fh:
        json.dump(results, fh, indent=1)
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
        fig, axs = plt.subplots(1, 2, figsize=(10, 3.8))
        for ax, (fs, ma, mb, se), title in zip(axs, [curves1, curves2],
                                               ["single node", f"{g}x{g} grid (mean over nodes)"]):
            ax.semilogx(fs, ma, label="nftsim", color="#1f4e79")
            ax.semilogx(fs, mb, label="JAX", color="#d9822b", ls="--")
            ax.fill_between(fs, ma - 2 * se, ma + 2 * se, color="#1f4e79", alpha=0.15, lw=0)
            ax.set_xlabel("frequency (Hz)")
            ax.set_ylabel("log10 PSD of phi_ee (s^-2 / Hz)")
            ax.set_title(title)
            ax.legend(frameon=False)
        fig.tight_layout()
        fig.savefig(os.path.join(args.out, "noise_spectra.png"), dpi=120)
    except ImportError:
        pass
