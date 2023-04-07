##############################################################################
# The CS 136 makefile                                                        #
#                                                                            #
# version 3.0b: Dave Tompkins (March 2023)                                   #
# version 1.0: Stacey Watson                                                 #
#                                                                            #
# If you find any bugs or have suggestions, email dtompkins@uwaterloo.ca     #
##############################################################################

CS136VERSION = "CS 136 makefile -- version 3.0b -- beta"

# this allows us to use > instead of [tabs] to avoid spaces vs. tabs editor issues
.RECIPEPREFIX = >

version: FORCE
> @echo $(CS136VERSION)

help: FORCE
> @echo "------------------------------------------------------------------------------"
> @echo $(CS136VERSION)
> @echo "------------------------------------------------------------------------------"
> @echo "To use this makefile, you will typically want to:"
> @echo
> @echo "make               build your program"
> @echo "make test          test with all .in files (see below)"
> @echo "make mytest.in     test just a single .in file (see below)"
> @echo "make clean         removes the executable and the results folder"
> @echo "make vscode        generate support files for vscode [BETA]"
> @echo "make submit        submit this question to marmoset [BETA]"
> @echo "make usage         help on how to use the makefile"
> @echo "make verbose       build and show the full build command"
> @echo "------------------------------------------------------------------------------"
> @echo "make test (in more detail)"
> @echo
> @echo "1) create a ./results/ folder"
> @echo "2) for each .in file in your testing folder(s) (e.g., testname.in):"
> @echo "   a) execute your program and generate testname.out and testname.trace files"
> @echo "      testname.out    contains all stdout (e.g., printf output)"
> @echo "      testname.trace  contains all tracing output (e.g., trace_int)"
> @echo "                      and it also contains any runtime error messages"
> @echo "   b) if no runtime errors occurred and it did not timeout, then"
> @echo "   c) compare your testname.out to your testname.expect (if it exists)"
> @echo "   d) if there are any differences, generate a testname.diff file"
> @echo "------------------------------------------------------------------------------"
> @echo "to customize the behaviour of this makefile, you may override some variables"
> @echo "for example: make test TESTDIRS=\"testdir1 testdir2\" TIMEOUT=120 MONO=1"
> @echo
> @echo "  TIMEOUT=            do not use a timeout (endless loops run forever)"
> @echo "  TIMEOUT=sec         timeout each test after sec seconds [default: $(TIMEOUT)]"
> @echo "  MAIN=filename.c     use a different main (client) file"
> @echo "  EXEC=filename       change the executable name"
> @echo "  TESTDIRS=d1 d2      specify an alternate directories for *.in files"
> @echo "  RESULTS=dirname     specify an alternate directory for test results"
> @echo "  MONO=1              do not use colour"
> @echo "------------------------------------------------------------------------------"

usage: FORCE
> @echo "------------------------------------------------------------------------------"
> @echo $(CS136VERSION)
> @echo "------------------------------------------------------------------------------"
> @echo "Here is the typical use of this makefile:"
> @echo
> @echo "  MAIN = main.c"
> @echo "  MODULES = module1.c module2.o ../common/assn_module.c /any/dir/module.o"
> @echo "  TESTDIRS = tests /any/dir/ ."
> @echo "  MARMOSET_PROJECT = a5q3"
> @echo
> @echo "  # include this makefile (AFTER the above definitions):"
> @echo "  include" $(abspath $(lastword $(MAKEFILE_LIST)))
> @echo
> @echo "optional makefile variables:"
> @echo
> @echo "  MAIN      contains the location of the .c file with main()"
> @echo "            [if MAIN is not set, it builds with *.c in the current directory]"
> @echo "  MODULES   any additional required modules (.c or .o files)"
> @echo "            [modules from" $(LIB) "are added automatically]"
> @echo "  EXEC      the name of the program to build"
> @echo "            [defaults to myprogram]"
> @echo "  TESTDIRS  the directories containing .in and .expect files"
> @echo "            [defaults to tests]"
> @echo
> @echo "  MARMOSET_PROJECT  the name of the assignment question in Marmoset"
> @echo "                    [required for make submit]"
> @echo "  MARMOSET_FILES    the files and directories/ to submit to Marmoset"
> @echo "                    [defaults to *]"
> @echo "------------------------------------------------------------------------------"

##############################################################################
# coloured output... define MONO=1 to prevent any colour escape codes
colEXEC ?= \033[1;36m
colFILE ?= \033[1;34m
colPASS ?= \033[1;32m
colFAIL ?= \033[1;31m
colNONE ?= \033[0;0m
print_colour = $(if $(MONO),,$(2))$(1)$(if $(MONO),,$(colNONE))
##############################################################################

# tell make to use one instance of bash:
SHELL = /bin/bash
.ONESHELL:

# recipes that depend on FORCE always execute, regardless of file state
FORCE:

# default to myprogram if EXEC is not set elsewhere
EXEC ?= myprogram

# default target is EXEC if not specified when running make
.DEFAULT_GOAL = $(EXEC)

# default timeout is 5 seconds ?!?! TODO
TIMEOUT ?= 5
TIMEOUT_EXEC = $(if $(TIMEOUT),timeout $(TIMEOUT),)

# cs136 library directory and object files
LIB ?= /u2/cs136/lib/w23/common/
LIB_OBJS = $(wildcard $(LIB)*.o)

# if MAIN is not set, then just grab all .c files in current directory
MAIN ?= $(wildcard *.c)

# all the sources -- remove possible duplicates
SOURCES = $(sort $(LIB_OBJS) $(MODULES) $(MAIN))
C_SOURCES = $(filter %.c,$(SOURCES))

# include paths from all modules
INC = $(sort $(addprefix -I,$(dir $(LIB) $(MODULES))))

# interfaces for the modules (.h dependencies)
INTERFACES = $(addsuffix .h,$(basename $(LIB_OBJS) $(MODULES)))

# use "clang" as the C Compilter (CC)
CC = clang

# Simple compiler flags
CFLAGS ?= -std=c99 -g -Wall -Werror

# Marmoset behaviour compiler flags
MARMOSET_CFLAGS ?= -fno-omit-frame-pointer -fno-common -fsanitize=address -Werror=int-conversion -Werror=int-to-pointer-cast -Werror=return-type -Werror=import-preprocessor-directive-pedantic -Werror=incompatible-pointer-types -Wvla -O0

# put all the build commands together
BUILD = $(CC) $(CFLAGS) $(MARMOSET_CFLAGS) $(INC) $(SOURCES) -o $(EXEC)

# check integrity statement
INTEGRITY_CHECK = 1

# regex to detect if the integrity statement has not been modified
INTEGRITY_REGEX = ERROR_NO_INTEGRITY_STATEMENT|ERROR_NO_NAME|ERROR_NO_LOGIN

##############################################################################

# recipe to build the main program
$(EXEC): $(SOURCES) $(INTERFACES)
> @printf "Building [$(call print_colour,$(EXEC),$(colEXEC))] from [$(call print_colour,$(sort $(notdir $(SOURCES))),$(colFILE))] ...\n"
> @if [ "$(INTEGRITY_CHECK)" == "1" ] && egrep "$(INTEGRITY_REGEX)" $(C_SOURCES) >/dev/null ; then printf "$(call print_colour,REJECT,$(colFAIL)): missing or incomplete integrity statement in [$(call print_colour,$(shell egrep -l "$(INTEGRITY_REGEX)" $(C_SOURCES)),$(colFILE))]\n" ; exit 71; fi
> @$(BUILD) ; _BUILD_RET=$$?
> @if [[ $$_BUILD_RET -eq 0 ]] ; then printf "... $(call print_colour,SUCCESS,$(colPASS))\n"; fi

# use "make verbose" to build and see the full build command
verbose: showcc $(EXEC)

# just show the build command
showcc: FORCE
> @echo $(BUILD)

##############################################################################

# default testing directory is tests, which corresponds to ./tests/
TESTDIRS ?= tests

# default results directory is ./results/
RESULTS ?= results
ifeq (/,$(lastword $(subst /,/ /,$(RESULTS))))
  $(error You may specify a RESULTS directory (currently [$(RESULTS)]) but do not add a trailing /)
endif

# find all .in files (without path)
INFILES = $(foreach TDIR,$(TESTDIRS),$(wildcard $(TDIR)/*.in))

# diff parameters
DIFF_PARAMS ?= --side-by-side --width 140

# recipe to test the .in files
%.in: $(EXEC) FORCE
> @if [[ ! -d $(RESULTS) ]] ; then mkdir $(RESULTS) ; fi
# the following is a tricky way to support both "make tests/file.in" and "make file.in"
> @$(eval _IN = $(if $(wildcard $@),$@,$(if $(filter %/$@,$(INFILES)),$(firstword $(filter %/$@,$(INFILES))),$@)))
> @if [[ ! -e $(_IN) ]] ; then printf "$(call print_colour,FILE NOT FOUND: could not locate [$(_IN)],$(colFAIL))\n" ; exit 4 ; fi
> @$(eval _EXPECT = $(basename $(_IN)).expect)
> @$(eval _STEM = $(notdir $(basename $(_IN))))
> @$(eval _OUT = $(RESULTS)/$(_STEM).out)
> @$(eval _TRACE = $(RESULTS)/$(_STEM).trace)
> @$(eval _DIFF = $(RESULTS)/$(_STEM).diff)
> @echo "--------------------"
> @printf "testing [$(call print_colour,$(_IN),$(colEXEC))] generating: [$(call print_colour,$(_OUT),$(colFILE))] [$(call print_colour,$(_TRACE),$(colFILE))]\n"
> @rm -f $(_OUT) $(_TRACE) $(_DIFF)
# the $$(...) is there to gobble the makefile error when an assert failure occurs
> @$$($(TIMEOUT_EXEC) $(abspath $(EXEC)) < $(_IN) 1> $(_OUT) 2> $(_TRACE)) ; _RETCODE=$$?
> @if [[ $$_RETCODE -ne 0 && $$_RETCODE -ne 124 && $$_RETCODE -ne 134 ]] ; then printf ">> $(call print_colour,ERROR,$(colFAIL)): exit code [$(call print_colour,$$_RETCODE,$(colFAIL))]: see [$(call print_colour,$(_TRACE),$(colFILE))] for details\n" ; _SUMM=$$(egrep "SUMMARY" $(_TRACE)); if [[ ! -z $$_SUMM ]] ; then printf ">> $(call print_colour,$$_SUMM,$(colFAIL))\n"; fi ; fi
> @if [[ $$_RETCODE -eq 124 ]] ; then printf ">> $(call print_colour,TIMEOUT,$(colFAIL)): terminated after $(TIMEOUT) seconds\n" ; fi
> @if [[ $$_RETCODE -eq 134 ]] ; then printf ">> $(call print_colour,ASSERT FAIL,$(colFAIL)): see [$(call print_colour,$(_TRACE),$(colFILE))] for details\n" ; printf ">> $(call print_colour,$$(egrep "Assertion" $(_TRACE)),$(colFAIL))\n" ; fi
> @if [[ $$_RETCODE -eq 0 && ! -e $(_EXPECT) ]] ; then printf ">> $(call print_colour,IGNORED,$(colFAIL)): no .expect file found: displaying [$(call print_colour,$(_OUT),$(colFILE))]:\n" ; cat $(_OUT) ; else diff $(_EXPECT) $(_OUT) >/dev/null ; _RETDIFF=$$? ; fi
> @if [[ $$_RETCODE -eq 0 && -e $(_EXPECT) && $$_RETDIFF == 1 ]] ; then diff $(DIFF_PARAMS) $(_EXPECT) $(_OUT) > $(_DIFF) ; printf ">> $(call print_colour,FAILED,$(colFAIL)): displaying [$(call print_colour,$(_DIFF),$(colFILE))] (with .expect on left):\n" ; cat $(_DIFF) ; fi
> @if [[ $$_RETCODE -eq 0 && -e $(_EXPECT) && $$_RETDIFF == 0 ]] ; then printf ">> $(call print_colour,PASSED,$(colPASS))\n" ; fi

# clean up files from building and test suite
clean: FORCE
> @rm -f $(EXEC)
> @rm -rf $(RESULTS)

# run the test suite
test: $(EXEC) $(INFILES) FORCE
ifeq (,$(INFILES))
> @echo "--------------------"
> @printf "$(call print_colour,NO TEST FILES (.in) FOUND,$(colFAIL)): just running [$(call print_colour,./$(EXEC),$(colEXEC))] (expecting stdin from keyboard)\n"
> @echo "--------------------"
> @$(abspath $(EXEC))
else
> @echo "--------------------"
endif

##############################################################################

VSCODE_SCRIPT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))vscode-init.py
VSCODE_PARAM_1 = $(abspath $(EXEC))
VSCODE_PARAM_2 = $(sort $(LIB) $(dir $(abspath $(MODULES))))
VSCODE_PARAM_3 = $(abspath $(INFILES))

vscode: FORCE
> @$(VSCODE_SCRIPT) "$(VSCODE_PARAM_1)" "$(VSCODE_PARAM_2)" "$(VSCODE_PARAM_3)"

##############################################################################

MARMOSET_COURSE ?= CS136
MARMOSET_FILES ?= *
MARMOSET_SCRIPT ?= /u/cs_build/bin/marmoset
MARMOSET_ZIP_PARAMS ?= -r
MARMOSET_EXCLUDE ?= makefile $(EXEC) .vscode/ .vscode/* $(RESULTS)/ $(RESULTS)/*

submit: FORCE $(EXEC)
> @if [ "$(MARMOSET_PROJECT)" == "" ] ; then printf "$(call print_colour,ERROR,$(colFAIL)): submit requries MARMOSET_PROJECT to be set\n" ; exit 3 ; fi
> @if [ "$(MARMOSET_PROJECT)" == "NONE" ] ; then printf "$(call print_colour,REJECT,$(colFAIL)): there is no marmset project configured (MARMOSET_PROJECT = NONE)\n" ; exit 0 ; fi
> @$(eval _ZIPFILE = $(shell tempfile --suffix=.zip))
> @rm $(_ZIPFILE)
> @zip $(MARMOSET_ZIP_PARAMS) $(_ZIPFILE) $(MARMOSET_FILES) -x $(MARMOSET_EXCLUDE)
> @$(MARMOSET_SCRIPT) submit $(MARMOSET_COURSE) $(MARMOSET_PROJECT) $(_ZIPFILE)
> @rm $(_ZIPFILE)

results: FORCE
> @if [ "$(MARMOSET_PROJECT)" == "" ] ; then printf "$(call print_colour,ERROR,$(colFAIL)): results requries MARMOSET_PROJECT to be set\n" ; exit 3 ; fi
> @if [ "$(MARMOSET_PROJECT)" == "NONE" ] ; then printf "$(call print_colour,REJECT,$(colFAIL)): there is no marmset project configured (MARMOSET_PROJECT = NONE)\n" ; exit 0 ; fi
> @$(MARMOSET_SCRIPT) results $(MARMOSET_COURSE) $(MARMOSET_PROJECT)

##############################################################################

tracevar = echo "[$(1)] == [$($(1))]"

diagnostics:
> @$(call tracevar,CS136VERSION)
> @$(call tracevar,.DEFAULT_GOAL)
> @$(call tracevar,.RECIPEPREFIX)
> @$(call tracevar,SHELL)
> @$(call tracevar,MONO)
> @$(call tracevar,EXEC)
> @$(call tracevar,TIMEOUT)
> @$(call tracevar,TIMEOUT_EXEC)
> @$(call tracevar,MAIN)
> @$(call tracevar,MODULES)
> @$(call tracevar,LIB)
> @$(call tracevar,LIB_OBJS)
> @$(call tracevar,SOURCES)
> @$(call tracevar,C_SOURCES)
> @$(call tracevar,INC)
> @$(call tracevar,INTERFACES)
> @$(call tracevar,CC)
> @$(call tracevar,CFLAGS)
> @$(call tracevar,MARMOSET_CFLAGS)
> @$(call tracevar,BUILD)
> @$(call tracevar,TESTDIRS)
> @$(call tracevar,INFILES)
> @$(call tracevar,RESULTS)
> @$(call tracevar,DIFF_PARAMS)
> @$(call tracevar,VSCODE_SCRIPT)
> @$(call tracevar,VSCODE_PARAM_1)
> @$(call tracevar,VSCODE_PARAM_2)
> @$(call tracevar,VSCODE_PARAM_3)
> @$(call tracevar,INTEGRITY_CHECK)
> @$(call tracevar,INTEGRITY_REGEX)
> @$(call tracevar,MARMOSET_SCRIPT)
> @$(call tracevar,MARMOSET_COURSE)
> @$(call tracevar,MARMOSET_PROJECT)
> @$(call tracevar,MARMOSET_FILES)
> @$(call tracevar,MARMOSET_ZIP_PARAMS)
> @$(call tracevar,MARMOSET_EXCLUDE)

##############################################################################
