
TOP = ..
SOURCE=\
	settable_toggle.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DSETTABLE_TOGGLE_TEST
APP=settable_toggle

include $(TOP)/mkfiles/test.mk
