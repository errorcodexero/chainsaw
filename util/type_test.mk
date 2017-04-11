
TOP = ..
SOURCE=\
	type.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DTYPE_TEST
APP=type

include $(TOP)/mkfiles/test.mk
