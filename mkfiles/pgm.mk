#
# This make file makes a library based on the list of source files.  This
# makefile depends on the following to be defined
#
# SOURCE - the list of source files to be part of the library
# TARGET - the target for the library (e.g ROBOT, LOCAL)
#

OBJDIR=$(APP)_objs
include $(TOP)/mkfiles/common.mk

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
# The program depends on the object files specified, which depends on the source files given
#
$(APPNAME) : dirs $(DEPFILES) $(OBJS)
	$(CXX) -o $(APPNAME) $(OBJS) $(LDFLAGS) $(PGMLIBS)

#
# Be sure the directories we need are created
#
dirs::
	mkdir -p $(DEPDIR)

#
# include the dependencies
#
-include $(DEPFILES)
