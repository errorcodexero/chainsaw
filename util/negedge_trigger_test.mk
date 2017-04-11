
TOP = ..
SOURCE=\
	negedge_trigger.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DNEGEDGE_TRIGGER_TEST
APP=negedge_trigger

include $(TOP)/mkfiles/test.mk
