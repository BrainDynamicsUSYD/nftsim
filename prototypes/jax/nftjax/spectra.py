"""Power spectra in JAX (differentiable), for fitting and for comparing with nftsim."""

import jax.numpy as jnp


def welch(x, fs, nperseg):
    """One-sided Welch power spectral density of x along its last axis.

    Hann window, 50% overlap, mean removed per segment; the same estimator as
    scipy.signal.welch(x, fs, nperseg=nperseg) with its defaults.
    Returns (f, psd) with psd in units of x^2/Hz.
    """
    n = x.shape[-1]
    step = nperseg // 2
    nseg = (n - nperseg) // step + 1
    idx = jnp.arange(nperseg)[None, :] + step * jnp.arange(nseg)[:, None]
    seg = x[..., idx]  # (..., nseg, nperseg)
    seg = seg - seg.mean(axis=-1, keepdims=True)
    w = 0.5 - 0.5 * jnp.cos(2 * jnp.pi * jnp.arange(nperseg) / nperseg)  # periodic Hann
    spec = jnp.abs(jnp.fft.rfft(seg * w, axis=-1)) ** 2 / (fs * jnp.sum(w ** 2))
    spec = spec.at[..., 1:].multiply(2.0)
    if nperseg % 2 == 0:
        spec = spec.at[..., -1].multiply(0.5)
    f = jnp.fft.rfftfreq(nperseg, 1.0 / fs)
    return f, spec.mean(axis=-2)


def band_power(f, psd, lo, hi):
    """Power in the band [lo, hi] Hz (rectangle rule on the PSD grid)."""
    df = f[1] - f[0]
    mask = (f >= lo) & (f <= hi)
    return jnp.sum(jnp.where(mask, psd, 0.0), axis=-1) * df
