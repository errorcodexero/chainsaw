
TOP = ..
SOURCE=\
	debounce_timer.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DDEBOUNCE_TIMER_TEST
APP=debounce_timer

include $(TOP)/mkfiles/test.mk
