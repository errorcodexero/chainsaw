
TOP = ..
SOURCE=\
	posedge_trigger.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DPOSEDGE_TRIGGER_TEST
APP=posedge_trigger

include $(TOP)/mkfiles/test.mk
