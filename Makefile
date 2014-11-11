# Makefile

LIBS = 


# Performance
COMP = g++ -g -lm -Wall -O3 -Wextra -pedantic -std=c++11 -msse -msse2 -msse3 -mfpmath=sse -march=native -mtune=native -funroll-loops -flto #-m64

# Debugging
#COMP = g++ -g -lm -Wall -Wextra -pedantic -std=c++11 -msse -msse2 -msse3

# Performance, parallel
#COMP = g++ -g -lm -Wall -O3 -Wextra -pedantic -std=c++11 -msse -msse2 -msse3 -mfpmath=sse -march=native -mtune=native -funroll-loops -flto -m64 -fopenmp

# Cross-compiling
#COMP = x86_64-w64-mingw32-g++ -lm -Wall -O3 -msse -msse2 -msse3 -mfpmath=sse -funroll-loops -flto -m64 -std=gnu++11 -static -static-libgcc -static-libstdc++

HEADER = $(wildcard *.h)
CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

default: Release/NeuroField

Release/NeuroField: $(addprefix Release/,$(OBJ))
	$(COMP) $(addprefix Release/,$(OBJ)) -o $@ $(LIBS)

$(addprefix Release/,$(OBJ)): Release/%.o: %.cpp %.h
	@mkdir -p Release
	@$(COMP) -c $< -o $@
	@echo "CC $<"

Documentation/user.pdf: Documentation/user.tex
	cd Documentation && pdflatex user && pdflatex user

Documentation/developer.pdf: Documentation/developer.tex
	cd Documentation && pdflatex developer && pdflatex developer

Paper/neurofield.pdf: Paper/neurofield.tex
	cd Paper && pdflatex neurofield && pdflatex neurofield

.PHONY: clean doc paper

doc: Documentation/user.pdf Documentation/developer.pdf

paper: Paper/neurofield.pdf

clean:
	echo Delete Release/ Documentation/doc
	-rm -r Release Documentation/{user,developer}.{aux,log,out,toc} Documentation/x.log

#.SILENT:

.INTERMEDIATE: main.h

main.h:
	touch main.h
