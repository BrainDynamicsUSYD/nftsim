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

__version__ = '0.1.4'
import logging
logging.basicConfig(filename='nftsim.log', level=logging.INFO,
                    format='%(asctime)s:%(levelname)s:%(message)s')
LOG = logging.getLogger(__name__)
import os
import sys

import numpy as np
import matplotlib.pyplot as plt

class NF:
    """
    Docstring for the NF class...
    """

    def __init__(self, nf_output_file):
        """
        Docstring for the init method
        """
        with open(nf_output_file, 'rU') as outfile:
            [conf, skip] = self.read_conf(outfile)

            headers = outfile.readline()
            nodes = outfile.readline()
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

        data = np.loadtxt(nf_output_file, dtype=np.float64, skiprows=skip)

        self.time = data[:, 0]
        start_idx = 1
        self.data = {}
        for i in range(0, len(self.fields)):
            stop_idx = start_idx  + len(self.nodes[self.fields[i]])
            self.data[self.fields[i]] = data[:, start_idx:stop_idx]
            start_idx = stop_idx

        self.deltat = self.time[1]-self.time[0]
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
        plt.plot(self.time, self.data[trace])
        plt.show()

    def read_conf(self, fid):
        """
        Read a .conf file...
        """
        l = fid.readline()
        skiprows = 0
        while not l.startswith('======================='):
            if 'Time  |' in l:
                LOG.error('Did you try to open an old style output file?')
                sys.exit()
            l = fid.readline()
            skiprows += 1
        fid.readline()
        skiprows += 4 # blank line, traces, node list
        conf = {}
        return (conf, skiprows)

def run(filename, nftsim_path='./bin/nftsim'):
    """
    Run nftsim and store output in NF object...
    """
    filename = filename.replace('.conf', '')
    cmd = '%s -i %s.conf -o %s.output' % (nftsim_path, filename, filename)
    LOG.info('Running nftsim with command: \n %s \n' % cmd)
    result = os.system(cmd)

    if result:
        LOG.error('The execution of NFTsim did not finish cleanly.')
        sys.exit()
    else:
        LOG.info('\nFinished. Output file was written to %s.output\n' % filename)

    nf_obj = NF('%s.output' % filename)
    return nf_obj
