#
# This make file makes a library based on the list of source files.  This
# makefile depends on the following to be defined
#
# SOURCE - the list of source files to be part of the library
# TARGET - the target for the library (e.g ROBOT, LOCAL)
#

OBJDIR=$(APP)_objs
include $(TOP)/mkfiles/common.mk

ifeq ($(OS),Windows_NT)
VALGRIND=sh -c
else
VALGRIND=valgrind
endif

#
# Create the list of local libraries we are dependent on based on
# the MYLIBS list
#
PGMLIBS=$(addprefix $(TARGETDIR)/, $(addsuffix .a,$(MYLIBS)))

#
# Define the complete archive name based on the LIBNAME and the target
#
APPNAME=$(TARGETDIR)/$(APP)

#
# Define the name of the test result
#
TESTRESULT=$(TARGETDIR)/logs/$(APP).log

#
# Run the test with the output in a log file
#
$(TESTRESULT): $(APPNAME)
	$(VALGRIND) $(APPNAME) > $(TARGETDIR)/logs/$(APP).log 2>&1

#
# The program depends on the object files specified, which depends on the source files given
#
$(APPNAME) : dirs $(OBJS)
	$(CXX) -o $(APPNAME) $(OBJS) $(LDFLAGS) $(PGMLIBS)

#
# Be sure the directories we need are created
#
dirs::
	mkdir -p $(DEPDIR)
	mkdir -p $(TARGETDIR)/logs
