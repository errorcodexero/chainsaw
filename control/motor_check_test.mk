
TOP = ..
SOURCE=\
	motor_check.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DMOTOR_CHECK_TEST
APP=motor_check_test

include $(TOP)/mkfiles/test.mk
