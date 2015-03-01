# Neurofield Python helper scripts
# Development in progress
# Executing and parsing of output file is fully functional
# Analysis routines are incomplete
#
# Example usage:
#
#	import neurofield
#	nf = neurofield.run('filename.conf','path_to_NeuroField')
#	print(nf)
#	nf.plot('pop.1.q')

import os,sys
import numpy as np
import matplotlib.pyplot as plt

class NF:

	def __init__(self,nf_output_file):
		with open(nf_output_file,'rU') as outfile:
			[conf,skip] = self.read_conf(outfile)

			headers = outfile.readline()
			nodes = outfile.readline()
			self.fields = []

			field_list = [x.strip() for x in headers.lower().split()]
			[self.fields.append(x.strip())  for x in headers.lower().split() if (x not in ['','|','time'] and x.strip() not in self.fields) ]
			
			node_idx = [int(y) for y in nodes.split()]
			
			
			self.nodes = {}
			for i in range(1,len(field_list)):
				if field_list[i] in self.nodes:
					self.nodes[field_list[i]].append(node_idx[i-1])
				else:
					self.nodes[field_list[i]] = [node_idx[i-1]]

		data = np.loadtxt(nf_output_file,dtype=np.float64,skiprows=skip)

		self.time = data[:,0]
		start_idx = 1
		self.data = {}
		for i in range(0,len(self.fields)):
			stop_idx = start_idx  + len(self.nodes[self.fields[i]])
			self.data[self.fields[i]] = data[:,start_idx:stop_idx]
			start_idx = stop_idx

		self.deltat = self.time[1]-self.time[0]
		self.npoints = len(self.time)

	def __repr__(self):
		out = ''
		out += 'NeuroField output\n'
		out += 'Traces: %s\n' % (', '.join(self.fields))
		out += 'Start time: %f\n' % (self.time[0])
		out += 'Stop time: %f\n' % (self.time[-1])
		out += 'deltat: %f\n' % (self.deltat)
		out += 'npoints: %d\n' % (len(self.time))
		out += 'Nodes: %s' % (' '.join([str(x) for x in self.nodes[self.fields[0]]]))
		return out

	def extract(self,trace,t=[],nodes=[]):
		# Return a matrix for a particular trace/field at a subset of the nodes
		return

	def nf_grid(self,trace):
		# This function returns a gridded version of a single parameter in a 3D matrix
		# e.g. a 9x9x100 system will have a 100x81 matrix contained in data. This function
		# returns a 9x9x100
		return

	def plot(self,trace,t=[],nodes=[]):
		plt.plot(self.time,self.data[trace])
		plt.show()

	def read_conf(self,fid):
		l = fid.readline()
		skiprows = 0
		while not(l.startswith('=======================')):
			if 'Time  |' in l:
				print('Did you try to open an old style output file?')
				sys.exit()
			l = fid.readline()
			skiprows+=1
		fid.readline()
		skiprows+=4 # blank line, traces, node list
		conf = {}
		return (conf,skiprows)

def run(filename,neurofield_path='./NeuroField'):
	filename = filename.replace('.conf','')
	result = os.system('%s -i %s.conf -o %s.output' % (neurofield_path,filename,filename))
	if result:
		print('Error! NeuroField did not exit cleanly')
		sys.exit()
	nf = NF('%s.output' % (filename))
	return nf


