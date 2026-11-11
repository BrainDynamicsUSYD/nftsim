"""Spread a batch of simulations over several devices (CPU cores or GPUs).

On CPU, XLA runs each simulation step on one thread, so to use all cores
the batch is split over several CPU "devices", made with

    XLA_FLAGS=--xla_force_host_platform_device_count=8

set before JAX is imported (see ../scripts/benchmark.py).  Each device then
runs batch_simulate() on its share of the batch in its own thread.
"""

import numpy as np
import jax
import jax.numpy as jnp
from jax.sharding import Mesh, PartitionSpec as P

from .model import batch_simulate, default_record


def sharded_batch_simulate(spec, pbatch, n_steps, out_every=1, keys=None,
                           record=default_record, unroll=1, layout="vmap", devices=None):
    """batch_simulate() with the batch split evenly over devices (default: all).

    The batch size must be a multiple of the number of devices.
    """
    devices = devices if devices is not None else jax.devices()
    nd = len(devices)
    B = jax.tree_util.tree_leaves(pbatch)[0].shape[0]
    if B % nd:
        raise ValueError(f"batch size {B} is not a multiple of {nd} devices")
    if keys is None:
        keys = jax.random.split(jax.random.PRNGKey(0), B)
    mesh = Mesh(np.array(devices), ("b",))

    def local(pb, kk):
        return batch_simulate(spec, pb, n_steps, out_every, kk, record, unroll, layout)

    f = jax.shard_map(local, mesh=mesh, in_specs=(P("b"), P("b")), out_specs=P("b"),
                      check_vma=False)
    return f(pbatch, keys)
