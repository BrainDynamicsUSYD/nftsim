import os
import sys
import commands

def file_subst(fname,s1,s2):
	f = open(fname,'r')
	temp = f.read().split('\n')
	f.close()
	f = open(fname, 'w')
	for line in temp:
		f.write(line.replace(s1,s2)+'\n')
	f.close()

def prepend(fname):
	f = open(fname,'r')
	temp = f.read()
	f.close()

	f = open(fname, 'w')
	f.write("#define WAVEFOURIER\n")

	f.write(temp)
	f.close()

def rmline(fname):
	f = open(fname,'r')
	temp = f.read()
	f.close()

	temp = temp.split('\n')
	temp = temp[1:]
	f = open(fname, 'w')
	for line in temp:
		f.write(line+'\n')
	f.close()

if len(sys.argv)==1:
	print "Usage:\n\n fftw_mode enable/disable\n\n to activate or deactivate FFTW usage"

enabled = len(commands.getoutput('grep wavefourier Makefile'))==0

if sys.argv[1] == 'enable':
	if enabled:
		print "Already enabled"
		sys.exit()
	file_subst('Makefile','#wavefourier LIBS','LIBS')
	prepend('solver.cpp')
	prepend('wavefourier.cpp')
else:
	if not enabled:
		print "Already disabled"
		sys.exit()
	file_subst('Makefile','LIBS','#wavefourier LIBS')
	rmline('solver.cpp')
	rmline('wavefourier.cpp')
