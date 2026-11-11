# -*- coding: utf-8 -*-

"""
  Defines a NFTsim Python class that supports
  + running a configuration file
  + loading the output file for visualization and analysis

USAGE:
    import nftsim
    nfobj = nftsim.run('filename.conf','path_to_nftsim_executable')
    print(nfobj)
    this_trace = 'propagator.1.phi' # traces have the form class.obj_index.variable
    nfobj.plot(this_trace) 

OPTIONS:

EXAMPLES:
    # From the root nftsim directory
    import utils.nftsim as nftsim
    nfobj = nftsim.run('configs/example.conf', './bin/nftsim')
    print(nfobj)
    nfobj.plot('propagator.1.phi')

REQUIRES:
  We recommend Anaconda on Linux.
  Python 3 or a recent python 2.7.
  numpy 
  matplotlib

.. moduleauthor:: Stuart Knock <>
.. moduleauthor:: Romesh Abeysuriya <>
.. moduleauthor:: John Griffiths <>
"""

__version__ = '0.2.0'
import logging
# A library should not configure logging (this used to create nftsim.log in
# the working directory on import, and reconfigure the root logger); use
# logging.basicConfig in your own script to see these messages.
LOG = logging.getLogger(__name__)
LOG.addHandler(logging.NullHandler())
import os
import re
import subprocess
import warnings

import numpy as np

class NF:
    """
    Docstring for the NF class...
    """

    def __init__(self, nf_output_file):
        """
        Read an nftsim output file, in the text or a binary format (detected
        from the file itself).

        The attributes are the same for both formats: time, data, data_dict,
        fields, nodes, deltat (the output sampling interval) and npoints, plus
        format ('text' or 'binary') and metadata (the key=value pairs of a
        binary file's NFTSIM-BINARY line; empty for text). Values are float64
        in every case (binary32 values are converted).

        A run that was interrupted (or is still running) leaves a last row
        that is incomplete: it is dropped with a warning.
        """
        with open(nf_output_file, 'rb') as outfile:
            [conf, skip] = self.read_conf(outfile)

            headers = outfile.readline().decode('utf-8', 'replace')
            nodes = outfile.readline().decode('utf-8', 'replace')
            self.fields = []

            field_list = [x.strip() for x in headers.lower().split()]
            [self.fields.append(x.strip())  for x in headers.lower().split() if (x not in ['', '|', 'time'] and x.strip() not in self.fields) ]

            node_idx = [int(y) for y in nodes.split()]

            self.nodes = {}
            for i in range(1, len(field_list)):
                if field_list[i] in self.nodes:
                    self.nodes[field_list[i]].append(node_idx[i-1])
                else:
                    self.nodes[field_list[i]] = [node_idx[i-1]]

            line = outfile.readline()
            data_offset = outfile.tell()

        self.metadata = {}
        if line.startswith(BINARY_MAGIC):
            self.format = 'binary'
            self.metadata = parse_binary_metadata(line)
            data = read_binary_rows(nf_output_file, data_offset, self.metadata,
                                    len(field_list))
        else:
            self.format = 'text'
            data = read_text_rows(nf_output_file, skip)

        self.time = data[:, 0]
        start_idx = 1
        self.data = {}
        for i in range(0, len(self.fields)):
            stop_idx = start_idx  + len(self.nodes[self.fields[i]])
            self.data[self.fields[i]] = data[:, start_idx:stop_idx]
            start_idx = stop_idx

        if len(self.time) > 1 or 'interval' not in self.metadata:
            self.deltat = self.time[1]-self.time[0]
        else:
            self.deltat = float(self.metadata['interval'])
        self.npoints = len(self.time)

        self.data_dict = {}
        for k, v in self.nodes.items():
            self.data_dict[k] = {vv: self.data[k][:, vv_it] for vv_it, vv in enumerate(v)}

    def __repr__(self):
        out = ''
        out += 'NFTsim output\n'
        out += 'Traces: %s\n' % (', '.join(self.fields))
        out += 'Start time: %f\n' % (self.time[0])
        out += 'Stop time: %f\n' % (self.time[-1])
        out += 'deltat: %f\n' % (self.deltat)
        out += 'npoints: %d\n' % (len(self.time))
        out += 'Nodes: %s' % (' '.join([str(x) for x in self.nodes[self.fields[0]]]))
        return out

    def extract(self, trace, t=[], nodes=[]):
        """
        Return a matrix for a particular trace/field at a subset of the nodes.
        """
        raise NotImplementedError

    def nf_grid(self, trace):
        """
        This function returns a gridded version of a single parameter in a
        3D matrix e.g. a 9x9x100 system will have a 100x81 matrix contained in
        data. This function returns a 9x9x100. 
        + TODO: this function will need 'Longside'
        """
        raise NotImplementedError

    def plot(self, trace):
        """
        Plot trace...
        + TODO: this overlaps with functionality included in configs_compare
        """
        # Imported here so that reading output needs only numpy.
        import matplotlib.pyplot as plt
        plt.plot(self.time, self.data[trace])
        plt.show()

    def read_conf(self, fid):
        """
        Skip the configuration echoed at the top of an output file, up to and
        including the blank line after the line of '='. fid may be open in
        text or binary mode. Returns ({}, the number of lines before the data
        rows of a text file).
        """
        l = fid.readline()
        if isinstance(l, bytes):
            decode = lambda line: line.decode('utf-8', 'replace')
            l = decode(l)
        else:
            decode = lambda line: line
        skiprows = 0
        while not l.startswith('======================='):
            if l == '':
                raise ValueError('%s is not an nftsim output file: no line of '
                                 "'=' separates the configuration from the data."
                                 % getattr(fid, 'name', 'the file'))
            if 'Time  |' in l:
                raise ValueError('%s looks like an old-style output file, which '
                                 'this reader does not support.'
                                 % getattr(fid, 'name', 'the file'))
            l = decode(fid.readline())
            skiprows += 1
        fid.readline()
        skiprows += 4 # blank line, traces, node list
        conf = {}
        return (conf, skiprows)


# A binary output file has the same header as a text one, then a line that
# starts with this, and then the rows (see the user manual, "Binary output").
BINARY_MAGIC = b'NFTSIM-BINARY'
BINARY_DTYPES = {'float64': '<f8', 'float32': '<f4'}


def parse_binary_metadata(line):
    """The key=value pairs of a NFTSIM-BINARY line, as a dict of strings."""
    meta = dict(item.split('=', 1) for item in line.decode('ascii').split()[1:])
    if meta.get('version') != '1':
        raise ValueError('Unsupported nftsim binary format version %s.' % meta.get('version'))
    if meta.get('byteorder') != 'little' or meta.get('dtype') not in BINARY_DTYPES:
        raise ValueError('Unsupported nftsim binary dtype %s or byteorder %s.'
                         % (meta.get('dtype'), meta.get('byteorder')))
    return meta


def read_binary_rows(path, offset, meta, ncols):
    """Rows of a binary output file as a float64 array of shape (rows, ncols)."""
    if int(meta['columns']) != ncols:
        raise ValueError('%s: the header names %d columns but the binary metadata says %s.'
                         % (path, ncols, meta['columns']))
    values = np.fromfile(path, dtype=BINARY_DTYPES[meta['dtype']], offset=offset)
    nrows = values.size // ncols
    partial = os.path.getsize(path) - offset - nrows * ncols * values.itemsize
    if meta['rows'] == 'stream':
        # Written to standard output or a pipe, so the count was never known.
        if partial:
            warnings.warn('%s ends with a partial row: read %d complete rows.' % (path, nrows))
    elif meta['rows'] == 'unknown':
        warnings.warn('%s is incomplete (the run was interrupted or is still running): '
                      'read %d complete rows%s.'
                      % (path, nrows, ', dropped a partial row' if partial else ''))
    elif int(meta['rows']) != nrows or partial:
        warnings.warn('%s should have %s rows but holds %d complete rows%s; read those.'
                      % (path, meta['rows'], nrows, ' and a partial row' if partial else ''))
        nrows = min(nrows, int(meta['rows']))
    return values[:nrows * ncols].reshape(nrows, ncols).astype(np.float64)


def read_text_rows(path, skiprows):
    """Rows of a text output file. A last line without a newline is the
    incomplete row of an interrupted run, so it is dropped with a warning."""
    with open(path, 'rb') as f:
        f.seek(-1, os.SEEK_END)
        complete = f.read(1) == b'\n'
    if complete:
        return np.loadtxt(path, dtype=np.float64, skiprows=skiprows, ndmin=2)
    with open(path, 'rb') as f:
        nrows = f.read().count(b'\n') - skiprows
    warnings.warn('%s ends with an incomplete row (the run was interrupted or is '
                  'still running): read %d complete rows.' % (path, nrows))
    return np.loadtxt(path, dtype=np.float64, skiprows=skiprows, max_rows=nrows, ndmin=2)


def _config_sets_format(conf):
    """True if the configuration file's Output section sets the output Format."""
    try:
        with open(conf) as f:
            text = f.read()
    except OSError:
        return False  # nftsim itself reports a missing config
    # The Output section is the last one; use the last 'Output:' label, so a
    # mention in the header comment is ignored.
    starts = [m.start() for m in re.finditer(r'(^|\n)\s*Output:', text)]
    return bool(starts) and re.search(r'(^|\s)Format:', text[starts[-1]:]) is not None


def run(filename, nftsim_path='./bin/nftsim', output_format=None):
    """
    Run nftsim and store output in NF object...

    output_format is 'binary' (float64), 'binary32' or 'text'. If None,
    binary output is written (faster to write and read, and exact), unless
    the configuration file's Output section sets its own Format, which is then
    used. NF reads every format.
    """
    filename = filename.replace('.conf', '')
    cmd = [nftsim_path, '-i', '%s.conf' % filename, '-o', '%s.output' % filename]
    if output_format is None:
        if not _config_sets_format('%s.conf' % filename):
            cmd += ['--format', 'binary']
    else:
        if str(output_format).lower() not in ('text', 'binary', 'binary64', 'binary32'):
            raise ValueError("output_format must be 'binary', 'binary32' or 'text', not %r"
                             % (output_format,))
        cmd += ['--format', str(output_format)]
    LOG.info('Running nftsim with command: %s', ' '.join(cmd))
    # A list of arguments (no shell), so paths may contain spaces. A failure
    # raises an exception instead of exiting the interpreter (eg a notebook).
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            universal_newlines=True)
    if result.returncode != 0:
        raise RuntimeError('nftsim failed (exit status %d):\n%s'
                           % (result.returncode, result.stderr.strip()))
    LOG.info('Finished. Output file was written to %s.output', filename)

    nf_obj = NF('%s.output' % filename)
    return nf_obj
