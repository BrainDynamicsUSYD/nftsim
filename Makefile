#Makefile for nftsim
#
#  Type 'make help' for a list of possible targets with brief descriptions.
#  If you're having problems, the Makefile itself can be debugged using the
#  option -d, eg 'make -d help'. Also 'make info' will display the contents
#  of key variables in the Makefile, to help with debugging.
#
#  Variables that can be set on the command line:
#    ARCH     Target-architecture flags. The default (-march=native, plus
#             -mtune=native and SSE flags on Linux) optimises for the build
#             machine's CPU, so the binary may not run on an older or different
#             CPU (eg, another node type on a cluster). For a portable binary
#             use 'make ARCH=' or name a baseline, eg 'make ARCH=-march=x86-64-v2'.
#    FPFLAGS  Extra floating-point flags, empty by default. 'make
#             FPFLAGS=-ffp-contract=off' stops the compiler fusing a*b + c into
#             fused multiply-adds, so clang and g++ builds at any optimisation
#             level give bit-identical output (and match the regression
#             references exactly); it costs about 14% in run time on Apple M1.
#    LDLIBS   Libraries to link (default -lm).
#    OPENMP   'make OPENMP=1' builds with OpenMP, running the per-node loops on
#             several threads (prototype; the number of threads is set by
#             OMP_NUM_THREADS). Results are bit-for-bit the same as the serial
#             build for any number of threads. g++ needs nothing extra; Apple
#             clang needs libomp (eg 'brew install libomp'), found under
#             LIBOMP (default: Homebrew's prefix), or use g++, eg
#             'make OPENMP=1 CXX=g++-16'.
#  Objects are not rebuilt when only these flags change, so run 'make clean'
#  first, eg 'make clean && make ARCH='.
#

#Specify our directories.
SRCDIR := src/
OBJDIR := obj/
BINDIR := bin/
DOCDIR := doc/
DEPDIR := dep/

#Default to *nix, suffix-less, binary.
BIN := nftsim

#User-manual files
USER_MANUAL := NFTsimManual.pdf
USER_MANUAL_SRC := NFTsimManual.tex

#Default to *nix commands.
MV := mv -f
RM := rm -f
RMDIR := rm -rf
GREP := egrep
CAT := cat

# Standard Linux (gcc must be >= 4.8.5) performance
ifeq ($(shell uname -s), Linux)
  CXX := g++
  ARCH ?= -msse -msse2 -msse3 -mfpmath=sse -march=native -mtune=native
  CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic $(ARCH) $(FPFLAGS) -funroll-loops -flto -O3
  DEBUG := -std=c++11 -ggdb3 -Og -Wall -Wextra -pedantic $(ARCH) $(FPFLAGS) -funroll-loops
endif

# Mac OS, default to clang++ (GNU flags if CXX is set to g++, eg CXX=g++-16)
ifeq ($(shell uname -s), Darwin)
  CXX := clang++
  ARCH ?= -march=native
  ifneq (,$(findstring clang,$(shell $(CXX) --version 2>/dev/null | head -n 1)))
    CXXFLAGS := -std=c++11 -Wall -Wno-padded -Wno-c++98-compat -Wno-c++98-compat-pedantic -fdiagnostics-fixit-info -Wdocumentation $(ARCH) $(FPFLAGS) -funroll-loops -flto -O3
    DEBUG := -std=c++11 -g -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -fdiagnostics-fixit-info -Wdocumentation $(FPFLAGS)
  else
    CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic $(ARCH) $(FPFLAGS) -funroll-loops -flto -O3
    DEBUG := -std=c++11 -ggdb3 -Og -Wall -Wextra -pedantic $(ARCH) $(FPFLAGS) -funroll-loops
  endif
endif

# Libraries, passed to the linker (not the compiler).
LDLIBS ?= -lm

# Optional OpenMP build (make OPENMP=1). Apple clang has no -fopenmp driver
# support, so pass it to the preprocessor and link Homebrew's libomp.
ifeq ($(OPENMP),1)
  ifneq (,$(findstring clang,$(shell $(CXX) --version 2>/dev/null | head -n 1)))
    ifeq ($(shell uname -s), Darwin)
      LIBOMP ?= $(firstword $(wildcard /opt/homebrew/opt/libomp /usr/local/opt/libomp))
      OMPFLAGS := -Xpreprocessor -fopenmp -I$(LIBOMP)/include
      OMPLIBS := -L$(LIBOMP)/lib -lomp
    else
      OMPFLAGS := -fopenmp
    endif
  else
    OMPFLAGS := -fopenmp
  endif
  CXXFLAGS += $(OMPFLAGS)
  DEBUG += $(OMPFLAGS)
  LDLIBS += $(OMPLIBS)
endif

# Source and header files
HEADER := $(wildcard $(SRCDIR)*.h)
CPP := $(wildcard $(SRCDIR)*.cpp)
OBJ := $(addprefix $(OBJDIR),$(notdir $(CPP:.cpp=.o)))
DEP := $(addprefix $(DEPDIR),$(notdir $(CPP:.cpp=.d)))

#Targets that don't need the dependencies evaluated/included.
NO_DEPS := make_bin_dir make_obj_dir make_dep_dir help help-dev docs info $(DOCDIR)$(USER_MANUAL) reference-manual user-manual
NO_DEPS += clean-user-manual clean-reference-manual clean-docs clean-deps clean-objs clean-bin clean clean-all clean-test

# Delete the default suffixes
.SUFFIXES:
# Define our suffix list
.SUFFIXES: .o .d .cpp .h .tex

.PHONY: nftsim debug all clang make_bin_dir make_obj_dir make_dep_dir help help-dev info docs user-manual reference-manual \
	clean-user-manual clean-reference-manual clean-docs clean-deps clean-objs clean-bin clean clean-all \
	test test-unit test-numerical clean-test

# target: nftsim - Compile nftsim placing the executable in the bin directory.
nftsim: $(BINDIR)$(BIN)

# target: debug - Compile nftsim with debugging enabled.
debug: CXXFLAGS := $(DEBUG)
debug: nftsim

# target: all - Compile nftsim and build all documentation.
all: nftsim docs

#   target: gcc - Build using gcc, unnecessary on Linux as gcc is default.
ifeq ($(MAKECMDGOALS), gcc)
  CXX := $(shell command -v g++ 2> /dev/null)
  ifndef CXX
    $(error "You don't appear to have g++ installed. If it is installed make sure it's in your PATH.")
  endif
  CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic $(ARCH) $(FPFLAGS) -funroll-loops -flto -O3
endif
gcc: nftsim

#   target: clang - Build using clang++, unnecessary on MacOS unless you want extra warnings as clang++ is default.
ifeq ($(MAKECMDGOALS), clang)
  CXX := $(shell command -v clang++ 2> /dev/null)
  ifndef CXX
    $(error "You don't appear to have clang++ installed. If it is installed make sure it's in your PATH.")
  endif
  CXXFLAGS := -std=c++11 -Weverything -Wno-padded -Wno-c++98-compat -Wno-c++98-compat-pedantic -fdiagnostics-fixit-info -Wdocumentation $(ARCH) $(FPFLAGS) -funroll-loops -flto -O3
endif
clang: nftsim

#   target: intel - Build using intel C++ compiler.
ifeq ($(MAKECMDGOALS), intel)
  CXX := $(shell command -v icpc 2> /dev/null)
  ifndef CXX
    $(error "You don't appear to have icpc installed. If it is installed make sure it's in your PATH.")
  endif
  #TODO: consider/test -ipp -mkl -unroll-aggressive
  #NOTE: -fast => -static -ipo -xHost -O3... build system needs dev stuff for -static to work...
  CXXFLAGS := -std=c++11 -Wall -Wremarks -Wchecks -Weffec++ -xHost $(FPFLAGS) -funroll-loops -ipo -O3
endif
intel: nftsim

#   target: $(BINDIR)$(BIN) - Main target for the final build, linking objects into an executable.
$(BINDIR)$(BIN): $(OBJ) | make_bin_dir
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDLIBS)
	@$(CAT) conditions.txt
	@echo "USE OF NFTSIM CONSTITUTES ACCEPTANCE OF THE APACHE 2.0 LICENSE"

#   target: make_bin_dir - Create the directory $BINDIR if it doesn't already exist.
make_bin_dir:
	@test -d $(BINDIR) || { mkdir $(BINDIR) ; echo "mkdir $(BINDIR)"; }

#   target: make_obj_dir - Create the directory $OBJDIR if it doesn't already exist.
make_obj_dir:
	@test -d $(OBJDIR) || { mkdir $(OBJDIR) ; echo "mkdir $(OBJDIR)"; }

#   target: make_dep_dir - Create the directory $DEPDIR if it doesn't already exist.
make_dep_dir:
	@test -d $(DEPDIR) || { mkdir $(DEPDIR) ; echo "mkdir $(DEPDIR)"; }

# Header dependencies: each object's dep/*.d file is written by the compiler
# as a side effect of compiling it (-MMD), so it always lists the headers the
# object was actually built from, including headers newly included by other
# headers. (They used to be generated by a separate rule that only reran when
# the .cpp changed, which could leave a stale object after a header gained an
# #include.) -MP adds a dummy rule per header so deleting a header does not
# break the build. Include existing dependency files when the target needs them.
DEPGEN = -MMD -MP -MF $(DEPDIR)$*.d -MT $@
ifeq (, $(filter $(MAKECMDGOALS), $(NO_DEPS)))
-include $(DEP)
endif

# Build object code
$(OBJDIR)%.o: $(SRCDIR)%.cpp | make_obj_dir make_dep_dir
	$(CXX) $(CXXFLAGS) $(DEPGEN) -c $< -o $@

# Unit tests: test/unit/*.cpp linked against every nftsim object except main.
UNIT_DIR := test/unit/
UNIT_SRC := $(wildcard $(UNIT_DIR)*.cpp)
UNIT_HEADER := $(wildcard $(UNIT_DIR)*.h)
UNIT_BIN := $(BINDIR)unit_tests
LIB_OBJ := $(filter-out $(OBJDIR)main.o,$(OBJ))

# target: test - Build nftsim and the unit tests, then run the unit and numerical tests.
test: test-unit test-numerical

# target: test-unit - Build and run the unit tests in test/unit/.
test-unit: $(UNIT_BIN)
	./$(UNIT_BIN)

# target: test-numerical - Check the output of every test/numerical/*.conf against its reference.
test-numerical: nftsim
	./nf_numerical --check all

#   target: $(UNIT_BIN) - Link the unit tests.
$(UNIT_BIN): $(UNIT_SRC) $(UNIT_HEADER) $(HEADER) $(LIB_OBJ) | make_bin_dir
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -I$(UNIT_DIR) $(UNIT_SRC) $(LIB_OBJ) -o $@ $(LDLIBS)

#   target: clean-test - Delete the unit-test executable.
clean-test:
	-$(RM) $(UNIT_BIN)

# target: help - Print this message.
help:
	@echo "Available Makefile targets, call as 'make target', eg. make all"
	@$(GREP) "^# target:" Makefile

# target: help-dev - Print an extended list of targets.
help-dev: help
	@echo "Additional targets, mainly useful for developers."
	@$(GREP) "^#   target:" Makefile

#   target: info - A convenience target for debugging the Makefile.
info:
	@echo "  SHELL: $(SHELL)"
	@echo "  MAKE_VERSION: $(MAKE_VERSION)"
	@echo "  .VARIABLES: $(.VARIABLES)"
	@echo "  .DEFAULT_GOAL: $(.DEFAULT_GOAL)"
	@echo "  SRCDIR: $(SRCDIR)"
	@echo "  OBJDIR: $(OBJDIR)"
	@echo "  BINDIR: $(BINDIR)"
	@echo "  DOCDIR: $(DOCDIR)"
	@echo "  HEADER: $(HEADER)"
	@echo "  CPP: $(CPP)"
	@echo "  OBJ: $(OBJ)"
	@echo "  DEP: $(DEP)"
	@echo "  CXX: $(CXX)"
	@echo "  CXXFLAGS: $(CXXFLAGS)"
	@echo "  ARCH: $(ARCH)"
	@echo "  FPFLAGS: $(FPFLAGS)"
	@echo "  LDLIBS: $(LDLIBS)"
	@echo "  AR: $(AR)"
	@echo "  LD: $(LD)"
	@echo "  DEBUG: $(DEBUG)"
	@echo "  NO_DEPS: $(NO_DEPS)"

# target: docs - Build user and reference manuals.
docs: user-manual reference-manual

# target: user-manual - Build only the user-manual.
user-manual: $(DOCDIR)$(USER_MANUAL)

#   target: $(DOCDIR)$(USER_MANUAL) - target that actually builds the user manual.
$(DOCDIR)$(USER_MANUAL): $(DOCDIR)$(USER_MANUAL_SRC)
	cd $(DOCDIR)                && \
	pdflatex $(USER_MANUAL_SRC) && \
	pdflatex $(USER_MANUAL_SRC)

# target: reference-manual - Build only the reference-manual.
reference-manual:
	doxygen Doxyfile

# target: clean-docs - Delete temporary LaTeX files and Doxygen generated reference-manual.
clean-docs: clean-user-manual clean-reference-manual

# target: clean-user-manual - Delete temporary LaTeX files generated when building the user-manual.
clean-user-manual:
	-$(RM) $(DOCDIR)$(USER_MANUAL_SRC:.tex=).{aux,log,out,toc}

# target: clean-reference-manual - Delete Doxygen generated reference-manual.
clean-reference-manual:
	-$(RMDIR) $(DOCDIR)html
	-$(RMDIR) $(DOCDIR)latex

#   target: clean-deps - Delete the dependencies created during build.
clean-deps:
	-$(RM) $(DEP)

#   target: clean-objs - Delete the objects created during build.
clean-objs:
	-$(RM) $(OBJ)

#   target: clean-bin - Delete the executable generated by the build.
clean-bin:
	-$(RM) $(BINDIR)$(BIN)

# target: clean - Delete dependency files, temporary build objects and temporary LaTeX files.
clean: clean-deps clean-objs clean-user-manual

# target: clean-all - Same as clean but also delete Doxygen generated docs, and the executable.
clean-all: clean-deps clean-objs clean-bin clean-docs
