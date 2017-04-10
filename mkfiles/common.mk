#
# Stuff common to pgm, lib, and test
#


#
# Make sure I have a list of source files
#
ifndef SOURCE
$(error SOURCE is not defined)
endif

#
# Make sure my target is legal
#
ifeq ($(TARGET), ROBOT)
else
ifeq ($(TARGET), LOCAL)
ifdef NEED_WPILIB
$(error TARGET is LOCAL but NEED_WPILIB is set.  There is no WPILIB for the local target (yet).)
endif
else
$(error TARGET is not defined or defined to something other than 'ROBOT' or 'LOCAL')
endif
endif

#
# define the target directory for object files and libraries
#
TARGETDIR=$(TOP)/build/$(TARGET)

#
# Include a make file that sets up the tools.  It sets up the cross compile environment
# if the target is ROBOT, or sets up the local compile environment if the target is
# LOCAL
#
include $(TOP)/mkfiles/tools.mk

#
# If the calling makefile needs the WPI lib, include it
#
ifdef NEED_WPILIB
include $(TOP)/mkfiles/wpi.mk
endif

#
# Define the list of object files from the list of source files
#
TMPOBJS=$(SOURCE:.cpp=.o)
OBJS = $(addprefix $(TARGETDIR)/$(OBJDIR)/, $(TMPOBJS))

#
# Define the dependency files
#
TMPDEPS=$(SOURCE:.cpp=.d)
DEPFILES=$(addprefix $(TARGETDIR)/$(OBJDIR)/.d/, $(TMPDEPS))

#
# Define the directory for the dependencies and be sure it exists
#
DEPDIR=$(TARGETDIR)/$(OBJDIR)/.d
$(shell mkdir -p $(DEPDIR))

#
# Copy in any CPP flags from the library make file
#
CPPFLAGS += $(MYCFLAGS)

#
# Define the recipe for compiling a single c plus plus file to an object
# file in the right place with the right options
#
$(TARGETDIR)/$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

#
# Define the recipe for generating dependencies
#
$(TARGETDIR)/$(OBJDIR)/.d/%.d: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MF $@ -MT $(subst .d/,,$(basename $@)).o $<

