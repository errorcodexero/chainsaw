
TOP = ..
SOURCE=\
	interface.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DINTERFACE_TEST
APP=interface

include $(TOP)/mkfiles/test.mk
