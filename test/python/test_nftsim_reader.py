"""Tests for the Python output reader, utils/nftsim.py.

Reads reference outputs from test/numerical/data with nftsim.NF and checks the
traces, node indices, times and values against an independent parse of the
same file. Binary output is checked in two ways: files written here from the
text references (always run), and files written by bin/nftsim, compared with
its text output of the same run (skipped if bin/nftsim has not been built).
Files cut off mid-row, as an interrupted run leaves them, are also read.
Run from the repository root with

    python3 -m unittest discover -s test/python -v

Needs numpy only (matplotlib is imported only for plotting).
"""

import os
import re
import subprocess
import sys
import tempfile
import unittest
import warnings

import numpy as np

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'utils'))
os.chdir(tempfile.mkdtemp())  # nftsim.py writes nftsim.log in the working directory
import nftsim  # noqa: E402

DATA = os.path.join(ROOT, 'test', 'numerical', 'data')
CONFS = os.path.join(ROOT, 'test', 'numerical')
NFTSIM = os.path.join(ROOT, 'bin', 'nftsim')
TMP = tempfile.mkdtemp()


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


def text_tokens(path):
    """The data rows of a text output file, as lists of strings."""
    with open(path) as f:
        lines = f.read().splitlines()
    sep = max(i for i, l in enumerate(lines) if l.startswith('====='))
    return [l.split() for l in lines[sep + 4:]]


def write_binary(text_path, binary_path, dtype='float64', rows=None):
    """Write the binary form of a text output file, following the format
    described in the user manual (independently of nftsim's writer)."""
    with open(text_path, 'rb') as f:
        lines = f.read().split(b'\n')
    sep = max(i for i, l in enumerate(lines) if l.startswith(b'====='))
    header = b'\n'.join(lines[:sep + 4]) + b'\n'
    values = np.loadtxt(text_path, skiprows=sep + 4, ndmin=2)
    meta = ('NFTSIM-BINARY version=1 dtype=%s byteorder=little columns=%d '
            'deltat=1.0e-04 interval=%.16e rows=%-20s\n'
            % (dtype, values.shape[1], values[1, 0] - values[0, 0],
               values.shape[0] if rows is None else rows))
    with open(binary_path, 'wb') as f:
        f.write(header + meta.encode('ascii'))
        f.write(values.astype('<f8' if dtype == 'float64' else '<f4').tobytes())


def truncate(path, nbytes):
    """Cut nbytes off the end of a file, as an interrupted run can leave it."""
    with open(path, 'rb+') as f:
        f.truncate(os.path.getsize(path) - nbytes)


def run_nftsim(conf, output, *args):
    subprocess.check_call([NFTSIM, '-i', conf, '-o', output] + list(args),
                          stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


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

    def test_truncated_text(self):
        # Cut in the middle of the last number: that row is dropped, with a warning.
        path = os.path.join(TMP, 'cut.output')
        with open(os.path.join(DATA, 'e-erps.output'), 'rb') as f:
            whole = f.read()
        with open(path, 'wb') as f:
            f.write(whole[:-5])
        full = nftsim.NF(os.path.join(DATA, 'e-erps.output'))
        with self.assertWarns(UserWarning):
            cut = nftsim.NF(path)
        self.assertEqual(cut.npoints, full.npoints - 1)
        np.testing.assert_array_equal(cut.time, full.time[:-1])
        for field in full.fields:
            np.testing.assert_array_equal(cut.data[field], full.data[field][:-1])

    def test_not_an_output_file(self):
        with tempfile.NamedTemporaryFile('w', suffix='.output', delete=False) as f:
            f.write('just a config\nTime: 1 Deltat: 0.1\n')
        with self.assertRaises(ValueError):
            nftsim.NF(f.name)
        os.remove(f.name)


def assert_same_output(test, a, b):
    """a and b hold the same traces, nodes, times and values."""
    test.assertEqual(a.fields, b.fields)
    test.assertEqual(a.nodes, b.nodes)
    test.assertEqual(a.npoints, b.npoints)
    test.assertEqual(a.deltat, b.deltat)
    np.testing.assert_array_equal(a.time, b.time)
    for field in a.fields:
        np.testing.assert_array_equal(a.data[field], b.data[field])
        test.assertEqual(sorted(a.data_dict[field]), sorted(b.data_dict[field]))
        for node in a.data_dict[field]:
            np.testing.assert_array_equal(a.data_dict[field][node], b.data_dict[field][node])


class TestBinaryReader(unittest.TestCase):
    """Binary files written from the text references, read back."""

    NAMES = ['e-erps', 'wave-rectangular-grid', 'eirs-dendrite-integral']

    def test_float64_matches_text(self):
        for name in self.NAMES:
            text = nftsim.NF(os.path.join(DATA, name + '.output'))
            path = os.path.join(TMP, name + '.bin')
            write_binary(os.path.join(DATA, name + '.output'), path)
            binary = nftsim.NF(path)
            self.assertEqual((text.format, binary.format), ('text', 'binary'))
            self.assertEqual(binary.metadata['dtype'], 'float64')
            assert_same_output(self, text, binary)

    def test_float32_is_text_rounded_to_float32(self):
        text = nftsim.NF(os.path.join(DATA, 'eirs-dendrite-integral.output'))
        path = os.path.join(TMP, 'f32.bin')
        write_binary(os.path.join(DATA, 'eirs-dendrite-integral.output'), path, 'float32')
        binary = nftsim.NF(path)
        self.assertEqual(binary.data[binary.fields[0]].dtype, np.float64)
        for field in text.fields:
            np.testing.assert_array_equal(binary.data[field],
                                          text.data[field].astype(np.float32).astype(np.float64))

    def test_cut_mid_row(self):
        # The last row is incomplete: it is dropped, with a warning.
        full = nftsim.NF(os.path.join(DATA, 'e-erps.output'))
        for dtype in ['float64', 'float32']:
            path = os.path.join(TMP, 'cut.bin')
            write_binary(os.path.join(DATA, 'e-erps.output'), path, dtype)
            truncate(path, 5)
            with self.assertWarns(UserWarning):
                cut = nftsim.NF(path)
            self.assertEqual(cut.npoints, full.npoints - 1)
            np.testing.assert_allclose(cut.time, full.time[:-1])

    def test_rows_unknown(self):
        # A run that stopped before it could record its row count (or is still running).
        full = nftsim.NF(os.path.join(DATA, 'e-erps.output'))
        path = os.path.join(TMP, 'unknown.bin')
        write_binary(os.path.join(DATA, 'e-erps.output'), path, rows='unknown')
        with self.assertWarns(UserWarning):
            out = nftsim.NF(path)
        assert_same_output(self, full, out)
        truncate(path, 8 * 3 * 10 + 1)  # ten rows and part of another
        with self.assertWarns(UserWarning):
            out = nftsim.NF(path)
        self.assertEqual(out.npoints, full.npoints - 11)

    def test_no_rows(self):
        path = os.path.join(TMP, 'empty.bin')
        write_binary(os.path.join(DATA, 'e-erps.output'), path, rows='unknown')
        size = os.path.getsize(path) - 8 * 3 * 256
        truncate(path, 8 * 3 * 256)
        self.assertEqual(os.path.getsize(path), size)
        with self.assertWarns(UserWarning):
            out = nftsim.NF(path)
        self.assertEqual(out.npoints, 0)
        self.assertEqual(out.data['pop.2.q'].shape, (0, 1))


@unittest.skipUnless(os.access(NFTSIM, os.X_OK), 'bin/nftsim has not been built')
class TestNftsimBinary(unittest.TestCase):
    """nftsim's own binary output, against its text output of the same run."""

    NAMES = ['e-erps', 'wave-rectangular-grid', 'eirs-dendrite-integral',
             'stimulus-white-square-grid', 'coupling-map', 'eirs-coupling-labels']

    def run_formats(self, name):
        conf = os.path.join(CONFS, name + '.conf')
        paths = {}
        for fmt in ['Text', 'Binary', 'Binary32']:
            paths[fmt] = os.path.join(TMP, '%s.%s.output' % (name, fmt))
            run_nftsim(conf, paths[fmt], '--format', fmt)
        return paths

    def test_binary_is_text_unrounded(self):
        for name in self.NAMES:
            paths = self.run_formats(name)
            text = nftsim.NF(paths['Text'])
            binary = nftsim.NF(paths['Binary'])
            self.assertEqual(binary.metadata['rows'], str(text.npoints))
            # Same header bytes, then the metadata line, starting the data 8-byte aligned.
            with open(paths['Text'], 'rb') as f:
                text_bytes = f.read()
            with open(paths['Binary'], 'rb') as f:
                binary_bytes = f.read()
            meta_start = binary_bytes.index(b'\nNFTSIM-BINARY') + 1
            self.assertEqual(binary_bytes[:meta_start], text_bytes[:meta_start])
            self.assertEqual((binary_bytes.index(b'\n', meta_start) + 1) % 8, 0)
            # Each text value is the binary double printed with 15 significant digits.
            tokens = text_tokens(paths['Text'])
            columns = [binary.time] + [binary.data[f][:, k] for f in binary.fields
                                       for k in range(len(binary.nodes[f]))]
            for i, row in enumerate(tokens):
                self.assertEqual(row, ['%.14e' % c[i] for c in columns])
            # The text reader's values are those, read back.
            self.assertEqual(text.fields, binary.fields)
            self.assertEqual(text.nodes, binary.nodes)
            for field in text.fields:
                np.testing.assert_allclose(binary.data[field], text.data[field], rtol=5e-15, atol=0)
            binary32 = nftsim.NF(paths['Binary32'])
            for field in text.fields:
                np.testing.assert_array_equal(
                    binary32.data[field], binary.data[field].astype(np.float32).astype(np.float64))

    def test_config_format_and_command_line_override(self):
        with open(os.path.join(CONFS, 'e-erps.conf')) as f:
            conf_text = f.read()
        conf_text = re.sub(r'(?m)^(Output:.*)$', r'\1 Format: Binary', conf_text)
        conf = os.path.join(TMP, 'format.conf')
        with open(conf, 'w') as f:
            f.write(conf_text)
        out = os.path.join(TMP, 'format.output')
        run_nftsim(conf, out)
        self.assertEqual(nftsim.NF(out).format, 'binary')
        run_nftsim(conf, out, '--format', 'text')
        self.assertEqual(nftsim.NF(out).format, 'text')
        run_nftsim(conf, out, '--format', 'binary32')
        self.assertEqual(nftsim.NF(out).metadata['dtype'], 'float32')

    def test_bad_format(self):
        with self.assertRaises(subprocess.CalledProcessError):
            run_nftsim(os.path.join(CONFS, 'e-erps.conf'), os.path.join(TMP, 'bad.output'),
                       '--format', 'csv')



@unittest.skipUnless(os.access(NFTSIM, os.X_OK), 'bin/nftsim has not been built')
class TestRun(unittest.TestCase):
    """nftsim.run(): binary by default, Text on request, and the config's own
    Format respected when no format is given."""

    def conf(self, name, format_line=''):
        with open(os.path.join(CONFS, 'e-erps.conf')) as f:
            text = f.read()
        text = re.sub(r'(\n\s*Output:[^\n]*)', lambda m: m.group(1) + format_line, text, count=1)
        path = os.path.join(TMP, 'run-%s.conf' % name)
        with open(path, 'w') as f:
            f.write(text)
        return path

    def run_quiet(self, conf, *args):
        with open(os.devnull, 'w') as null:
            saved = os.dup(1), os.dup(2)
            os.dup2(null.fileno(), 1)
            os.dup2(null.fileno(), 2)
            try:
                return nftsim.run(conf, NFTSIM, *args)
            finally:
                os.dup2(saved[0], 1)
                os.dup2(saved[1], 2)

    def is_binary(self, conf):
        with open(conf.replace('.conf', '.output'), 'rb') as f:
            return b'NFTSIM-BINARY' in f.read()

    def test_default_is_binary_with_the_same_values_as_text(self):
        binary = self.run_quiet(self.conf('default'))
        self.assertTrue(self.is_binary(self.conf('default')))
        text = self.run_quiet(self.conf('text'), 'text')
        self.assertFalse(self.is_binary(self.conf('text')))
        self.assertEqual(binary.fields, text.fields)
        for field in binary.fields:
            scale = max(np.abs(text.data[field]).max(), np.finfo(float).tiny)
            self.assertLessEqual(np.abs(binary.data[field] - text.data[field]).max(), 1e-13 * scale)

    def test_config_format_is_respected(self):
        self.run_quiet(self.conf('cfgtext', ' Format: Text'))
        self.assertFalse(self.is_binary(self.conf('cfgtext', ' Format: Text')))
        conf32 = self.conf('cfg32', ' Format: Binary32')
        self.run_quiet(conf32)
        with open(conf32.replace('.conf', '.output'), 'rb') as f:
            self.assertIn(b'dtype=float32', f.read())

    def test_explicit_format_overrides_the_config(self):
        conf = self.conf('override', ' Format: Text')
        self.run_quiet(conf, 'binary')
        self.assertTrue(self.is_binary(conf))

    def test_unknown_format_is_rejected(self):
        with self.assertRaises(ValueError):
            nftsim.run(self.conf('bad'), NFTSIM, 'csv')

if __name__ == '__main__':
    unittest.main()
