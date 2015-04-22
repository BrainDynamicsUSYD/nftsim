# Standard Linux, performance
CC = g++
CFLAGS = -g -lm -Wall -O3 -Wextra -pedantic -std=c++11 -msse -msse2 -msse3 -mfpmath=sse -march=native -mtune=native -funroll-loops -flto #-m64

# Mac OS
# CC = g++-4.9
# CFLAGS = -lm -Wall -O3 -std=c++11 


# Windows
# CC = x86_64-w64-mingw32-g++
# CFLAGS = -lm -Wall -O3 -msse -msse2 -msse3 -mfpmath=sse -funroll-loops -flto -m64 -std=gnu++11 -static -static-libgcc -static-libstdc++

# Debugging
# CC = g++
# CFLAGS = g++ -g -lm -Wall -Wextra -pedantic -std=c++11 -msse -msse2 -msse3


HEADER = $(wildcard src/*.h)
CPP = $(wildcard src/*.cpp)
OBJ = $(addprefix obj/,$(notdir $(CPP:.cpp=.o)))

# target: default - compile bin/neurofield 
default: bin/neurofield

bin/neurofield: $(OBJ)
	@mkdir -p bin
	@echo "$(CC) $(CFLAGS) $(OBJ) -o $@"
	@$(CC) $(CFLAGS) $(OBJ) -o $@ || (echo "mycommand failed $$?"; exit 1)
	@echo "====="
	@cat license.txt
	@echo "====="
	@echo "USE OF NEUROFIELD CONSTITUTES ACCEPTANCE OF THE LICENSE CONDITIONS ABOVE"

# Include any existing dependencies in the build
-include $(OBJ:.o=.d)

# Build object code and also create obj/*.d header dependencies
obj/%.o: src/%.cpp
	@mkdir -p obj
	@$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) -MM $(CFLAGS) $< > obj/$*.d
	@echo "CC $<"
	@mv -f obj/$*.d obj/$*.d.tmp
	@sed -e 's|.*:|$@:|' < obj/$*.d.tmp > obj/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.d
	@rm -f obj/$*.d.tmp

Documentation/neurofield.pdf: Documentation/neurofield.tex
	cd Documentation && pdflatex neurofield && pdflatex neurofield

.PHONY: clean doc help

# target: help - Print this message
help:
	@egrep "^# target:" Makefile

# target: doc - compile Documentation/neurofield.pdf
doc: Documentation/neurofield.pdf

# target: clean - Delete ./bin/, ./obj/, and LaTeX temporary files in ./Documentation/
clean:
	@-rm -rf bin obj Documentation/{user,developer}.{aux,log,out,toc} Documentation/x.log
