
TOP = ..
SOURCE=\
	executive.cpp\
	teleop.cpp\
	autonomous.cpp\
	step.cpp\
	chain.cpp\
	align.cpp


MYLIBS=\
	pixycam\
	control\
	input\
	util

MYCFLAGS=-DEXECUTIVE_TEST
APP=executive_test

include $(TOP)/mkfiles/test.mk
