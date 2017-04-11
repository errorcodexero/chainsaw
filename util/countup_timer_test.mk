
TOP = ..
SOURCE=\
	countup_timer.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DCOUNTUP_TIMER_TEST
APP=countup_timer

include $(TOP)/mkfiles/test.mk
