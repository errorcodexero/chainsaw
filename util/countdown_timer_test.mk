
TOP = ..
SOURCE=\
	countdown_timer.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DCOUNTDOWN_TIMER_TEST
APP=countdown_timer

include $(TOP)/mkfiles/test.mk
