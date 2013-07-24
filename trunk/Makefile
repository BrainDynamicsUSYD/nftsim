# Makefile

# compile command for sun
#COMP = CC

# generic compile command, used in yossarian
#COMP = g++ -g -lm -Wall  # Debugging
COMP = g++ -lm -Wall -Ofast -Wextra -pedantic -std=c++11 # Performance
#COMP = g++ -lm -Wall -O3 -Wextra -pedantic -std=c++11 -fopenmp # Performance, parallel
LIBS = 
#COMP = x86_64-w64-mingw32-g++ -lm -Wall -O3 -std=c++11 # Cross compiling

# compile command on intel
#COMP = /usr/physics/intel/cce/bin/icc -p -g -Drestrict=__restrict__ -vec-report1 -O2 -lm

HEADER = $(wildcard *.h)
CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

default: Release/NeuroField

Release/NeuroField: $(addprefix Release/,$(OBJ))
	$(COMP) $(addprefix Release/,$(OBJ)) -o $@ $(LIBS)

$(addprefix Release/,$(OBJ)): Release/%.o: %.cpp %.h
	mkdir -p Release
	$(COMP) -c $< -o $@

Documentation/doc.pdf: Documentation/doc.tex
	cd Documentation && pdflatex $(shell basename $<) && pdflatex $(shell basename $<)

.PHONY: clean doc

doc: Documentation/doc.pdf

clean:
	echo Delete Release/ Documentation/doc
	-rm -r Release Documentation/doc.{aux,log,out,toc} Documentation/x.log

.SILENT:

.INTERMEDIATE: main.h

main.h:
	touch main.h
