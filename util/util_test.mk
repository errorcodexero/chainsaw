
TOP = ..
SOURCE=\
	util.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DUTIL_TEST
APP=util_test

include $(TOP)/mkfiles/test.mk
