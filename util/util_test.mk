
TOP = ..
SOURCE=\
	util.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DUTIL_TEST
APP=util

include $(TOP)/mkfiles/test.mk
