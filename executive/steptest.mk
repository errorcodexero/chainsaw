
TOP = ..
SOURCE = \
	autonomous.cpp\
	step.cpp\
	chain.cpp\
	align.cpp\
	timed.cpp\
	test.cpp\
	teleop.cpp\
	executive.cpp

MYLIBS = \
	control\
	input\
	util\
	pixycam

MYCFLAGS = -DSTEP_TEST
APP=step_test

include $(TOP)/mkfiles/test.mk
