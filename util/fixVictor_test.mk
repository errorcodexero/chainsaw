
TOP = ..
SOURCE=\
	fixVictor.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DFIXVICTOR_TEST
APP=fixVictor

include $(TOP)/mkfiles/test.mk
