#
# This make file makes a library based on the list of source files.  This
# makefile depends on the following to be defined
#
# SOURCE - the list of source files to be part of the library
# TARGET - the target for the library (e.g ROBOT, LOCAL)
#

OBJDIR=$(LIBNAME)
include $(TOP)/mkfiles/common.mk

#
# Define the complete archive name based on the LIBNAME and the target
#
ARNAME=$(TARGETDIR)/$(LIBNAME).a

#
# The first release target.  This will build the library
#
# The library depends on the object files specified, which depends on the source files given
#
$(ARNAME) : $(DEPFILES) $(OBJS)
	$(AR) r $(ARNAME) $(OBJS)

#
# Advertise the list of tests in this directory
#
testlist::
	@echo $(TESTLIST)

#
# Include the dependencies
#
-include $(DEPFILES)


