
TOP = ..
SOURCE=\
	point.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DPOINT_TEST
APP=point

include $(TOP)/mkfiles/test.mk
