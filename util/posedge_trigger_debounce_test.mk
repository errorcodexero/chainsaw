
TOP = ..
SOURCE=\
	posedge_trigger_debounce.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DPOSEDGE_TRIGGER_DEBOUNCE_TEST
APP=posedge_trigger_debounce

include $(TOP)/mkfiles/test.mk
