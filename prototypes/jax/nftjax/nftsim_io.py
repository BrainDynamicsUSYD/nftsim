"""Run the nftsim executable and read its output."""

import os
import subprocess
import tempfile

import numpy as np

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))


def nftsim_path():
    """The nftsim executable: $NFTSIM, else bin/nftsim of this repository."""
    return os.environ.get("NFTSIM", os.path.join(REPO, "bin", "nftsim"))


def read_output(path):
    """Parse an nftsim .output file into (time, {trace name: array (time, nodes)}, nodes).

    The data follow the last line of '=' (and a blank line): a row of trace names (eg
    Pop.1.Q, Propagator.1.phi, one per node column), a row of node numbers
    (1-based), and one row of numbers per time.
    """
    with open(path) as f:
        lines = f.read().splitlines()
    last = max(i for i, l in enumerate(lines) if l.startswith("====="))
    rest = [l for l in lines[last + 1:] if l.strip()]
    names = rest[0].split()
    nodes = [int(x) for x in rest[1].split()]
    data = np.loadtxt(rest[2:], ndmin=2)
    t = data[:, 0]
    traces = {}
    node_of = {}
    for j, name in enumerate(names[1:]):
        traces.setdefault(name, []).append(data[:, j + 1])
        node_of.setdefault(name, []).append(nodes[j])
    traces = {k: np.stack(v, axis=1) for k, v in traces.items()}
    return t, traces, node_of


def run(conf_text, workdir=None, exe=None):
    """Write conf_text to a file, run nftsim on it, and return read_output()."""
    exe = exe or nftsim_path()
    d = workdir or tempfile.mkdtemp(prefix="nftjax-")
    conf = os.path.join(d, "run.conf")
    out = os.path.join(d, "run.output")
    with open(conf, "w") as f:
        f.write(conf_text)
    res = subprocess.run([exe, "-i", conf, "-o", out], capture_output=True, text=True, cwd=d)
    if res.returncode != 0:
        raise RuntimeError(f"nftsim failed on {conf}:\n{res.stderr}{res.stdout}")
    return read_output(out)
