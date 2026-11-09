#!/usr/bin/env python3
"""Compare an nftsim output file against a reference output, within a tolerance.

USAGE:
    compare_output.py [--rtol R] [--atol A] [--quiet] reference.output candidate.output

An nftsim output file holds an echo of the configuration file, a line of '='
characters, then a data block: a row of trace names (eg 'Time', 'Pop.1.Q'), a
row of node indices, and one row of numbers per output time-step.

The two files match when:
  * the echoed configuration text is identical (ignoring trailing whitespace);
  * the trace names and node indices are identical;
  * they have the same number of data rows; and
  * every value satisfies |candidate - reference| <= atol + rtol * scale,
    where scale is the largest magnitude in that column of the reference.

Scaling by the column maximum (rather than by each value) means values that
pass through zero, or traces that are mostly zero with a brief response, are
not held to an impossible relative accuracy. The defaults (rtol=1e-9, atol=0)
accept the round-off differences between compilers, optimisation levels and
CPU architectures, but not genuine changes in the numerical results.

Exit status: 0 if the files match, 1 if they differ, 2 if a file could not be
read or parsed.

Uses only the Python 3 standard library.
"""

import argparse
import math
import os
import sys

DEFAULT_RTOL = 1e-9
DEFAULT_ATOL = 0.0


class ParseError(Exception):
    """Raised when a file does not look like an nftsim output file."""


def parse_output(path):
    """Read an nftsim output file.

    Returns (header, names, nodes, rows): the echoed configuration lines, the
    trace names, the node indices (as strings), and a list of rows of floats.
    """
    try:
        with open(path) as f:
            lines = f.read().splitlines()
    except OSError as err:
        raise ParseError("cannot read '{}': {}".format(path, err.strerror))

    separators = [i for i, line in enumerate(lines) if line.startswith("=====")]
    if not separators:
        raise ParseError("'{}' has no '=====' line separating the configuration "
                         "from the data".format(path))
    sep = separators[-1]
    header = [line.rstrip() for line in lines[:sep]]

    body = [line for line in lines[sep + 1:] if line.strip()]
    if len(body) < 2:
        raise ParseError("'{}' has no trace-name and node-index rows after the "
                         "'=====' line".format(path))
    names = body[0].split()
    nodes = body[1].split()
    # The Time column has no node index.
    if len(nodes) != len(names) - 1 or names[0] != "Time":
        raise ParseError("'{}': expected a 'Time' column then one node index per "
                         "trace, found {} names and {} node indices"
                         .format(path, len(names), len(nodes)))
    nodes = [""] + nodes

    rows = []
    for k, line in enumerate(body[2:]):
        fields = line.split()
        if len(fields) != len(names):
            raise ParseError("'{}': data row {} has {} values, expected {}"
                             .format(path, k + 1, len(fields), len(names)))
        try:
            rows.append([float(x) for x in fields])
        except ValueError:
            raise ParseError("'{}': data row {} is not numeric: {!r}"
                             .format(path, k + 1, line.strip()))
    return header, names, nodes, rows


def column_label(names, nodes, j):
    """A readable label for column j, eg 'Pop.1.Q (node 2000)'."""
    if nodes[j]:
        return "{} (node {})".format(names[j], nodes[j])
    return names[j]


def values_match(ref, new, tol):
    """True if new equals ref to within tol; NaN matches only NaN, inf only the same inf."""
    if math.isnan(ref) or math.isnan(new):
        return math.isnan(ref) and math.isnan(new)
    if math.isinf(ref) or math.isinf(new):
        return ref == new
    return abs(new - ref) <= tol


def compare(ref_path, new_path, rtol=DEFAULT_RTOL, atol=DEFAULT_ATOL, out=sys.stdout):
    """Compare two output files. Returns 0 (match), 1 (differ) or 2 (error)."""
    try:
        ref_header, ref_names, ref_nodes, ref_rows = parse_output(ref_path)
        new_header, new_names, new_nodes, new_rows = parse_output(new_path)
    except ParseError as err:
        print("ERROR: {}".format(err), file=out)
        return 2

    if ref_header != new_header:
        for i in range(max(len(ref_header), len(new_header))):
            a = ref_header[i] if i < len(ref_header) else "<end of header>"
            b = new_header[i] if i < len(new_header) else "<end of header>"
            if a != b:
                print("Configuration echo differs at line {}:".format(i + 1), file=out)
                print("  reference: {}".format(a), file=out)
                print("  candidate: {}".format(b), file=out)
                break
        return 1

    if ref_names != new_names or ref_nodes != new_nodes:
        ref_cols = [column_label(ref_names, ref_nodes, j) for j in range(len(ref_names))]
        new_cols = [column_label(new_names, new_nodes, j) for j in range(len(new_names))]
        print("Traces differ:", file=out)
        print("  reference has {} columns, candidate has {}".format(len(ref_cols), len(new_cols)),
              file=out)
        missing = [c for c in ref_cols if c not in new_cols]
        extra = [c for c in new_cols if c not in ref_cols]
        if missing:
            print("  missing from candidate: {}".format(", ".join(missing)), file=out)
        if extra:
            print("  not in reference: {}".format(", ".join(extra)), file=out)
        if not missing and not extra:
            print("  same traces in a different order", file=out)
        return 1

    if len(ref_rows) != len(new_rows):
        print("Number of data rows differs: reference has {}, candidate has {}"
              .format(len(ref_rows), len(new_rows)), file=out)
        n = min(len(ref_rows), len(new_rows))
        for i in range(n):
            if not values_match(ref_rows[i][0], new_rows[i][0], rtol * abs(ref_rows[-1][0])):
                print("  first differing Time is at row {}: reference {!r}, candidate {!r}"
                      .format(i + 1, ref_rows[i][0], new_rows[i][0]), file=out)
                break
        if ref_rows and new_rows:
            print("  reference ends at Time {!r}, candidate at Time {!r}"
                  .format(ref_rows[-1][0], new_rows[-1][0]), file=out)
        return 1

    ncols = len(ref_names)
    scales = [max([abs(r[j]) for r in ref_rows if math.isfinite(r[j])] or [0.0])
              for j in range(ncols)]
    tols = [atol + rtol * s for s in scales]

    n_bad = 0
    bad_cols = set()
    worst = None  # (badness, row, col)
    max_rel = 0.0  # largest |diff|/scale seen, for the report
    for i, (ref_row, new_row) in enumerate(zip(ref_rows, new_rows)):
        for j in range(ncols):
            a, b = ref_row[j], new_row[j]
            ok = values_match(a, b, tols[j])
            if math.isfinite(a) and math.isfinite(b):
                diff = abs(b - a)
                if scales[j] > 0:
                    max_rel = max(max_rel, diff / scales[j])
                badness = diff / tols[j] if tols[j] > 0 else (math.inf if diff > 0 else 0.0)
            else:
                badness = 0.0 if ok else math.inf
            if not ok:
                n_bad += 1
                bad_cols.add(j)
            if worst is None or badness > worst[0]:
                worst = (badness, i, j)

    if n_bad == 0:
        print("Match: {} rows x {} columns; largest difference {:.3g} of the column scale "
              "(rtol {:g}, atol {:g})".format(len(ref_rows), ncols, max_rel, rtol, atol), file=out)
        return 0

    _, i, j = worst
    a, b = ref_rows[i][j], new_rows[i][j]
    print("DIFFERS: {} of {} values in {} of {} columns exceed the tolerance "
          "(rtol {:g} x column max, atol {:g})."
          .format(n_bad, len(ref_rows) * ncols, len(bad_cols), ncols, rtol, atol), file=out)
    print("  worst column: {}".format(column_label(ref_names, ref_nodes, j)), file=out)
    print("  worst row:    {} of {} (Time = {!r})".format(i + 1, len(ref_rows), ref_rows[i][0]),
          file=out)
    print("  reference:    {!r}".format(a), file=out)
    print("  candidate:    {!r}".format(b), file=out)
    if math.isfinite(a) and math.isfinite(b):
        print("  difference:   {:.6g} (allowed {:.6g}; column max |reference| = {:.6g})"
              .format(abs(b - a), tols[j], scales[j]), file=out)
    if len(bad_cols) > 1:
        others = sorted(bad_cols - {j})
        labels = [column_label(ref_names, ref_nodes, k) for k in others[:5]]
        more = " and {} more".format(len(others) - 5) if len(others) > 5 else ""
        print("  other columns: {}{}".format(", ".join(labels), more), file=out)
    return 1


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="Compare an nftsim output file against a reference within a tolerance.")
    parser.add_argument("reference", help="reference (expected) output file")
    parser.add_argument("candidate", help="output file to check")
    parser.add_argument("--rtol", type=float, default=DEFAULT_RTOL,
                        help="tolerance relative to each column's largest reference "
                             "magnitude (default %(default)g)")
    parser.add_argument("--atol", type=float, default=DEFAULT_ATOL,
                        help="absolute tolerance added to the relative one (default %(default)g)")
    parser.add_argument("-q", "--quiet", action="store_true",
                        help="print nothing; report only through the exit status")
    args = parser.parse_args(argv)
    if args.rtol < 0 or args.atol < 0:
        parser.error("tolerances must be non-negative")

    out = open(os.devnull, "w") if args.quiet else sys.stdout
    try:
        return compare(args.reference, args.candidate, args.rtol, args.atol, out)
    finally:
        if args.quiet:
            out.close()


if __name__ == "__main__":
    sys.exit(main())
