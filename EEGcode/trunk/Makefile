# Makefile, put all files into Release/ rather than pwd
# using this is completely optional

# compile command for sun
#COMP = CC
#CP = perl -w -p -e 's/__restrict__/\/* __restrict__ *\/ /g' < $< > $@

# compile command using gcc
COMP = g++ -g -msse2 -ftree-vectorize -ftree-vectorizer-verbose=5 -lm -Wall
CP = cp $< $@

# compile command on intel
#COMP = /usr/physics/intel/cce/bin/icc -p -g -Drestrict=__restrict__ -vec-report1 -O2 -lm
#CP = cp $< $@

HEADER = $(wildcard *.h)
CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

default: Release/NeuroField

Release/NeuroField: $(addprefix Release/,$(HEADER)) $(addprefix Release/,$(OBJ))
	$(COMP) $(addprefix Release/,$(OBJ)) -o $@

$(addprefix Release/,$(OBJ)): Release/%.o: Release/%.cpp Release/%.h
	$(COMP) -c $< -o $@

$(addprefix Release/,$(CPP)): Release/%.cpp: %.cpp
	$(CP)

$(addprefix Release/,$(HEADER)): Release/%.h: %.h
	@mkdir -p Release
	cp $< $@

Release/main.h:
	touch Release/main.h

.PHONY: clean

# deletes all object files and exes
clean:
	-rm -rI Release
