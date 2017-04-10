#
# This make file makes a library based on the list of source files.  This
# makefile depends on the following to be defined
#
# SOURCE - the list of source files to be part of the library
# TARGET - the target for the library (e.g ROBOT, LOCAL)
#
include $(TOP)/mkfiles/common.mk

#
# Define the list of object files from the list of source files
#
TMPOBJS=$(SOURCE:.cpp=.o)
OBJS = $(addprefix $(TARGETDIR)/$(APP)_objs/, $(TMPOBJS))

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
# Copy in any CPP flags from the library make file
#
CPPFLAGS += $(MYCFLAGS)

#
# Run the test with the output in a log file
#
$(TESTRESULT): $(APPNAME)
	valgrind $(APPNAME) > $(TARGETDIR)/logs/$(APP).log 2>&1

#
# Define the recipe for compiling a single c plus plus file to an object
# file in the right place with the right options
#
$(TARGETDIR)/$(APP)_objs/%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

#
# The program depends on the object files specified, which depends on the source files given
#
$(APPNAME) : dirs $(OBJS)
	$(CXX) -o $(APPNAME) $(OBJS) $(LDFLAGS) $(PGMLIBS)

#
# Be sure the directories we need are created
#
dirs::
	mkdir -p $(TARGETDIR)/$(APP)_objs
	mkdir -p $(TARGETDIR)/logs
