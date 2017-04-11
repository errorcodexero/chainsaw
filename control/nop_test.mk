
TOP = ..
SOURCE=\
	nop.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DNOP_TEST
APP=nop_test

include $(TOP)/mkfiles/test.mk
