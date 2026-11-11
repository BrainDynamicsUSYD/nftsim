"""Prototype JAX implementation of nftsim's neural field models (see ../README.md)."""

from .model import (Connection, Spec, simulate, batch_simulate, stack_params,
                    init_state, make_step, default_record, steady_state)
from . import eirs, nftsim_io

__all__ = ["Connection", "Spec", "simulate", "batch_simulate", "stack_params",
           "init_state", "make_step", "default_record", "steady_state", "eirs", "nftsim_io"]
