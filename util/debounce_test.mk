
TOP = ..
SOURCE=\
	debounce.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DDEBOUNCE_TEST
APP=debounce

include $(TOP)/mkfiles/test.mk
