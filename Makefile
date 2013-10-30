# Makefile

# compile command for sun
#COMP = CC

# generic compile command, used in yossarian
COMP = g++ -g -lm -Wall -Wextra -pedantic -std=c++11 -msse -msse2 -msse3 -mfpmath=sse -march=native -mtune=native -funroll-loops -flto # Debugging
#COMP = g++ -g -lm -Wall -O3 -Wextra -pedantic -std=c++11 -msse -msse2 -msse3 -mfpmath=sse -march=native -mtune=native -funroll-loops -flto -m64  # Performance
#COMP = g++ -lm -Wall -O3 -Wextra -pedantic -std=c++11 -fopenmp # Performance, parallel
LIBS = 
#COMP = x86_64-w64-mingw32-g++ -lm -Wall -O3 -std=c++11 -msse -msse2 -msse3 -mfpmath=sse -march=native -mtune=native -funroll-loops -flto # Cross compiling

# compile command on intel
#COMP = icpc -std=c++11 -fast -xSSE4.2 -ipo -no-prec-div -static -opt-prefetch -unroll-aggressive -m64 

HEADER = $(wildcard *.h)
CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

default: Release/NeuroField

Release/NeuroField: $(addprefix Release/,$(OBJ))
	$(COMP) $(addprefix Release/,$(OBJ)) -o $@ $(LIBS)

$(addprefix Release/,$(OBJ)): Release/%.o: %.cpp %.h
	mkdir -p Release
	$(COMP) -c $< -o $@

Documentation/user.pdf: Documentation/user.tex Documentation/developer.tex
	cd Documentation && pdflatex user && pdflatex user
	cd Documentation && pdflatex developer && pdflatex developer

.PHONY: clean doc

doc: Documentation/user.pdf

clean:
	echo Delete Release/ Documentation/doc
	-rm -r Release Documentation/{user,developer}.{aux,log,out,toc} Documentation/x.log

.SILENT:

.INTERMEDIATE: main.h

main.h:
	touch main.h
