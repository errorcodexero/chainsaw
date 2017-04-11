
TOP = ..
SOURCE=\
	force_interface.cpp\
	force.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DFORCE_INTERFACE_TEST
APP=force_interface_test

include $(TOP)/mkfiles/test.mk
