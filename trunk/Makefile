# Makefile

# compile command for sun
#COMP = CC

# generic compile command, used in yossarian
COMP = g++ -g -lm -Wall

# compile command using gcc
#COMP = g++ -g -msse2 -ftree-vectorize -ftree-vectorizer-verbose=5 -lm -Wall

# compile command on intel
#COMP = /usr/physics/intel/cce/bin/icc -p -g -Drestrict=__restrict__ -vec-report1 -O2 -lm

HEADER = $(wildcard *.h)
CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

default: Release/NeuroField

Release/NeuroField: $(addprefix Release/,$(OBJ))
	$(COMP) $(addprefix Release/,$(OBJ)) -o $@

$(addprefix Release/,$(OBJ)): Release/%.o: %.cpp %.h
	mkdir -p Release
	$(COMP) -c $< -o $@

Documentation/doc.pdf: Documentation/doc.tex
	cd Documentation && pdflatex $(shell basename $<) && pdflatex $(shell basename $<)

.PHONY: clean doc

doc: Documentation/doc.pdf

clean:
	echo Delete Release/ Documentation/doc
	-rm -rI Release Documentation/doc.{aux,log,pdf,out,toc} Documentation/x.log

.SILENT:

.INTERMEDIATE: main.h

main.h:
	touch main.h
