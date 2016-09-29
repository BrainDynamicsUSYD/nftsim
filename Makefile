#Makefile for neurofield
#
#  Type 'make help' for a list of possible targets with brief descriptions.
#  If you're having problems, the Makefile itself can be debugged using the
#  option -d, eg 'make -d help'. Also 'make info' will display the contents
#  of key variables in the Makefile, to help with debugging.
#

#Default to *nix directories.
SRCDIR := src/
OBJDIR := obj/
BINDIR := bin/
DOCDIR := doc/
DEPDIR := dep/

#Default to *nix, suffix-less, binary.
BIN := neurofield

#User-manual files
USER_MANUAL := NeurofieldManual.pdf
USER_MANUAL_SRC := NeurofieldManual.tex

#Default to *nix commands.
MV := mv -f
RM := rm -f
RMDIR := rm -rf
GREP := egrep
CAT := cat

# Standard Linux (gcc must be > 4.9) performance
ifeq ($(shell uname -s), Linux)
  CXX := g++
  CXXFLAGS := -lm -Wall -O3 -Wextra -pedantic -std=c++11 -msse -msse2 -msse3 -mfpmath=sse -march=native -mtune=native -funroll-loops -flto
  DEBUG := -ggdb3 -Og -lm -Wall -Wextra -pedantic -std=c++11 -msse -msse2 -msse3
  DEPFLAGS = -MM -MP -std=c++11 -MT $(OBJDIR)$*.o
endif

# Mac OS, default to clang++
ifeq ($(shell uname -s), Darwin)
  CXX := clang++
  CXXFLAGS := -lm -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -fdiagnostics-fixit-info -Wdocumentation -O3 -std=c++11
  DEBUG := -glldb -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -fdiagnostics-fixit-info -Wdocumentation
  DEPFLAGS = -MM -MP -std=c++11 -MT $(OBJDIR)$*.o
endif

# Source and header files
HEADER := $(wildcard $(SRCDIR)*.h)
CPP := $(wildcard $(SRCDIR)*.cpp)
OBJ := $(addprefix $(OBJDIR),$(notdir $(CPP:.cpp=.o)))
DEP := $(addprefix $(DEPDIR),$(notdir $(CPP:.cpp=.d)))

#Targets that don't need the dependencies evaluated/included.
NO_DEPS := make_bin_dir make_obj_dir make_dep_dir help help-dev docs info $(DOCDIR)$(USER_MANUAL) reference-manual user-manual
NO_DEPS += clean-user-manual clean-reference-manual clean-docs clean-deps clean-objs clean-bin clean clean-all

# Delete the default suffixes
.SUFFIXES:
# Define our suffix list
.SUFFIXES: .o .d .cpp .h .tex

.PHONY: neurofield debug all clang make_bin_dir make_obj_dir make_dep_dir help help-dev info docs user-manual reference-manual \
	clean-user-manual clean-reference-manual clean-docs clean-deps clean-objs clean-bin clean clean-all

# target: neurofield - compile neurofield placing the executable in the bin directory.
neurofield: $(BINDIR)$(BIN)

# target: debug - compile neurofield with debugging enabled.
debug: CXXFLAGS := $(DEBUG)
debug: neurofield

# target: all - compile neurofield and build all documentation.
all: neurofield docs

#   target: clang - Build using clang++, redundant on MacOS as clang++ is default.
clang: CXX := clang++
clang: CXXFLAGS := -lm -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -fdiagnostics-fixit-info -Wdocumentation -O3 -std=c++11
clang: DEPFLAGS = -MM -MP -std=c++11 -MT $(OBJDIR)$*.o
clang: neurofield

#   target: icc - Build using intel C++ compiler. #TODO: consider/test -ipp -mkl -unroll-aggressive -static
icc: CXX := icc
icc: CXXFLAGS := -Wall -Wremarks -Wchecks -Weffec++ -xHost -O3 -ipo -funroll-loops -std=c++11
icc: DEPFLAGS = -MM -MP -std=c++11 -MT $(OBJDIR)$*.o
icc: neurofield

#   target: $(BINDIR)$(BIN) - Main target for the final build, linking objects into an executable.
$(BINDIR)$(BIN): $(OBJ) | make_bin_dir
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@
	@echo "====="
	@$(CAT) license.txt
	@echo "====="
	@echo "USE OF NEUROFIELD CONSTITUTES ACCEPTANCE OF THE LICENSE CONDITIONS ABOVE"

#   target: make_bin_dir - Create the directory $BINDIR if it doesn't already exist.
make_bin_dir:
	@test -d $(BINDIR) || { mkdir $(BINDIR) ; echo "mkdir $(BINDIR)"; }

#   target: make_obj_dir - Create the directory $OBJDIR if it doesn't already exist.
make_obj_dir:
	@test -d $(OBJDIR) || { mkdir $(OBJDIR) ; echo "mkdir $(OBJDIR)"; }

#   target: make_dep_dir - Create the directory $DEPDIR if it doesn't already exist.
make_dep_dir:
	@test -d $(DEPDIR) || { mkdir $(DEPDIR) ; echo "mkdir $(DEPDIR)"; }

#Only create and include dependencies if the current target requires them.
ifeq (, $(filter $(MAKECMDGOALS), $(NO_DEPS)))
#Create obj/*.d header dependencies
$(DEPDIR)%.d: $(SRCDIR)%.cpp | make_dep_dir
	$(CXX) $(DEPFLAGS) $< > $(DEPDIR)$*.d
# Include any existing dependencies in the build
-include $(DEP)
endif

# Build object code
$(OBJDIR)%.o: $(SRCDIR)%.cpp | make_obj_dir
	$(CXX) $(CXXFLAGS) -c $< -o $@

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

#   target: clean-objs -  Delete the objects created during build.
clean-objs:
	-$(RM) $(OBJ)

#   target: clean-bin - Delete the executable generated by the build.
clean-bin:
	-$(RM) $(BINDIR)$(BIN)

# target: clean - Delete dependency files, temporary build objects and temporary LaTeX files.
clean: clean-deps clean-objs clean-user-manual

# target: clean-all - Same as clean but also delete Doxygen generated docs, and the executable.
clean-all: clean-deps clean-objs clean-bin clean-docs
