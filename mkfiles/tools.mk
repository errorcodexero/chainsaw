#
# This make file sets the tools for a given target
#

ifeq ($(TARGET), ROBOT)
	CXX=arm-frc-linux-gnueabi-g++
else
ifeq ($(TARGET), LOCAL)
	CXX=g++
else
$(error TARGET is not defined or defined to something other than 'ROBOT' or 'LOCAL')
endif
endif

CXXFLAGS += -pthread -Wall -Wextra -Werror -O2 -g -std=c++14
