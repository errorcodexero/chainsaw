
TOP = ..
SOURCE=\
	perf_tracker.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DPERF_TRACKER_TEST
APP=perf_tracker

include $(TOP)/mkfiles/test.mk
