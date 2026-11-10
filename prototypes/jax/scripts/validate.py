"""Compare the JAX model with nftsim on deterministic (pulse-driven) configurations.

    python scripts/validate.py [--steps 8192] [--nftsim PATH] [--nftsim-ref PATH]

--nftsim-ref is a second nftsim build (eg made with FPFLAGS=-ffp-contract=off),
compared with the first to show the size of round-off differences between builds.
"""

import argparse
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from nftjax.compare import CASES, compare  # noqa: E402

ap = argparse.ArgumentParser()
ap.add_argument("--steps", type=int, default=8192)
ap.add_argument("--nftsim", default=None)
ap.add_argument("--nftsim-ref", default=None)
args = ap.parse_args()

print(f"{args.steps} steps of Deltat = 2^-13 s ({args.steps / 8192:g} s)")
hdr = f"| case | max relative difference, JAX vs nftsim | worst trace |"
if args.nftsim_ref:
    hdr += " nftsim vs nftsim (other FP flags) |"
print(hdr)
print("|" + "---|" * (hdr.count("|") - 1))
for name in CASES:
    err, where, ref = compare(name, args.steps, args.nftsim, args.nftsim_ref)
    row = f"| {name} | {err:.1e} | {where} |"
    if args.nftsim_ref:
        row += f" {ref:.1e} |"
    print(row, flush=True)
