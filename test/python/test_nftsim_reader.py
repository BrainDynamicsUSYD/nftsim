"""Tests for the Python output reader, utils/nftsim.py.

Reads reference outputs from test/numerical/data with nftsim.NF and checks the
traces, node indices, times and values against an independent parse of the
same file. Run from the repository root with

    python3 -m unittest discover -s test/python -v

Needs numpy only (matplotlib is imported only for plotting).
"""

import os
import sys
import tempfile
import unittest

import numpy as np

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'utils'))
os.chdir(tempfile.mkdtemp())  # nftsim.py writes nftsim.log in the working directory
import nftsim  # noqa: E402

DATA = os.path.join(ROOT, 'test', 'numerical', 'data')


def parse(path):
    """Independent parse: data follow the last line of '=', then a row of trace
    names, a row of node indices, then one row of numbers per time."""
    with open(path) as f:
        lines = f.read().splitlines()
    sep = max(i for i, l in enumerate(lines) if l.startswith('====='))
    rest = [l for l in lines[sep + 1:] if l.strip()]
    names = rest[0].split()
    nodes = [int(x) for x in rest[1].split()]
    rows = np.array([[float(x) for x in l.split()] for l in rest[2:]])
    return names, nodes, rows


class TestReader(unittest.TestCase):

    def check(self, name):
        path = os.path.join(DATA, name + '.output')
        names, nodes, rows = parse(path)
        out = nftsim.NF(path)
        np.testing.assert_array_equal(out.time, rows[:, 0])
        self.assertEqual(out.npoints, rows.shape[0])
        self.assertAlmostEqual(out.deltat, rows[1, 0] - rows[0, 0])
        col = 1
        expected_fields = []
        for trace in names[1:]:
            if trace.lower() not in expected_fields:
                expected_fields.append(trace.lower())
        self.assertEqual(out.fields, expected_fields)
        for field in out.fields:
            n = len(out.nodes[field])
            cols = [j for j, t in enumerate(names) if t.lower() == field]
            self.assertEqual(out.nodes[field], [nodes[j - 1] for j in cols])
            self.assertEqual(cols, list(range(col, col + n)))
            np.testing.assert_array_equal(out.data[field], rows[:, col:col + n])
            for k, node in enumerate(out.nodes[field]):
                np.testing.assert_array_equal(out.data_dict[field][node], rows[:, col + k])
            col += n
        self.assertEqual(col, rows.shape[1])

    def test_single_node_traces(self):
        self.check('e-erps')

    def test_several_nodes_per_trace(self):
        self.check('wave-rectangular-grid')

    def test_many_traces(self):
        self.check('eirs-dendrite-integral')

    def test_not_an_output_file(self):
        with tempfile.NamedTemporaryFile('w', suffix='.output', delete=False) as f:
            f.write('just a config\nTime: 1 Deltat: 0.1\n')
        with self.assertRaises(ValueError):
            nftsim.NF(f.name)
        os.remove(f.name)


if __name__ == '__main__':
    unittest.main()
