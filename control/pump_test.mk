
TOP = ..
SOURCE=\
	pump.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DPUMP_TEST
APP=pump_test

include $(TOP)/mkfiles/test.mk
