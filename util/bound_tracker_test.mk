
TOP = ..
SOURCE=\
	bound_tracker.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DBOUND_TRACKER_TEST
APP=bound_tracker

include $(TOP)/mkfiles/test.mk
