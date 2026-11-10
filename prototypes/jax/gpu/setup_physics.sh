#!/bin/bash
# One-off setup on the USyd physics cluster head node, run from this Mac as
#
#   ssh physics 'bash -s' < prototypes/jax/gpu/setup_physics.sh
#
# (the login shell is tcsh, so the script is fed to bash on stdin).  It puts
# everything under /taiji1/bful7291/nftsim-jax (home is nearly full): a clone
# of the prototype branch, and a uv venv (CPython 3.12) with JAX for CUDA 13.
# Re-running it updates the clone and the venv.  Then submit the job with
#
#   ssh physics 'bash -s' <<'END'
#   cd /taiji1/bful7291/nftsim-jax/nftsim/prototypes/jax && /opt/pbs/bin/qsub gpu/physics_h100.pbs
#   END
set -euo pipefail
BASE=${BASE:-/taiji1/bful7291/nftsim-jax}
REPO=${REPO:-https://github.com/DynamicsAndNeuralSystems/nftsim.git}
BRANCH=${BRANCH:-prototype-jax}
UV=${UV:-$HOME/.local/bin/uv}
JAX_VERSION=0.12.0.1

mkdir -p "$BASE"
if [ -d "$BASE/nftsim/.git" ]; then
  git -C "$BASE/nftsim" fetch -q origin "$BRANCH"
  git -C "$BASE/nftsim" checkout -q "$BRANCH"
  git -C "$BASE/nftsim" reset -q --hard "origin/$BRANCH"
else
  git clone -q --branch "$BRANCH" "$REPO" "$BASE/nftsim"
fi
git -C "$BASE/nftsim" log -1 --oneline

if [ ! -x "$BASE/venv/bin/python" ]; then
  "$UV" venv -q --python 3.12 "$BASE/venv"
fi
# uv's cache would otherwise go to home; keep it on /taiji1 too.
export UV_CACHE_DIR="$BASE/uv-cache"
"$UV" pip install -q --python "$BASE/venv/bin/python" \
  "jax[cuda13]==$JAX_VERSION" numpy pytest matplotlib
"$BASE/venv/bin/python" -c "import jax; print('jax', jax.__version__, '(GPUs are visible only inside a job)')"
