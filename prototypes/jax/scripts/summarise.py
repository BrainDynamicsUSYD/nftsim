"""Markdown tables from benchmark.py's JSON results.

    python scripts/summarise.py results/bench_*.json
"""

import json
import sys


def fmt(x):
    return "-" if x is None else (f"{x:.3g}" if x < 1000 else f"{x:,.0f}")


for path in sys.argv[1:]:
    with open(path) as fh:
        res = json.load(fh)
    rows = {}
    for r in res["rows"]:
        rows.setdefault(r["B"], {})[r["impl"]] = r
    jax_rows = [r for r in res["rows"] if r["impl"] == "jax"]
    nft_rows = [r for r in res["rows"] if r["impl"] == "nftsim"]
    jdesc = ""
    if jax_rows:
        j = jax_rows[-1]
        jdesc = f"JAX {j['backend']} ({j['device']}, up to {max(r['devices'] for r in jax_rows)} devices, {j['layout']} layout)"
    ndesc = f"nftsim (up to {max(r['procs'] for r in nft_rows)} processes)" if nft_rows else ""
    print(f"\n{res['model']}, {res['time_s']:g} s simulated per parameter set, "
          f"throughput in simulated s per wall s ({path.split('/')[-1]})\n")
    cols = ["batch"]
    if jax_rows:
        cols += [jdesc, "JAX compile (s, approx.)"]
    if nft_rows:
        cols += [ndesc]
    if jax_rows and nft_rows:
        cols += ["JAX / nftsim"]
    print("| " + " | ".join(cols) + " |")
    print("|" + "---|" * len(cols))
    for B in sorted(rows):
        j, n = rows[B].get("jax"), rows[B].get("nftsim")
        line = [str(B)]
        if jax_rows:
            note = f" ({B // j['chunk']} chunks)" if j and j["chunk"] < B else ""
            line += [(fmt(j["throughput"]) + note) if j else "-",
                     (f"{max(j['first_call_s'] - j['wall_s'], 0):.2g}" if j["chunk"] == B else "-")
                     if j else "-"]
        if nft_rows:
            line += [fmt(n["throughput"]) if n else "-"]
        if jax_rows and nft_rows:
            line += [f"{j['throughput'] / n['throughput']:.2g}" if j and n else "-"]
        print("| " + " | ".join(line) + " |")
