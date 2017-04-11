
TOP = ..
SOURCE=\
	panel.cpp\
	util.cpp


MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DPANEL_TEST
APP=panel_test

include $(TOP)/mkfiles/test.mk
