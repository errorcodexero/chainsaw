
TOP = ..
SOURCE=\
	drivebase.cpp\
	motor_check.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DDRIVEBASE_TEST
APP=drivebase_test

include $(TOP)/mkfiles/test.mk
