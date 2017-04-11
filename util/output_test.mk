
TOP = ..
SOURCE=\
	output.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DOUTPUT_TEST
APP=output

include $(TOP)/mkfiles/test.mk
