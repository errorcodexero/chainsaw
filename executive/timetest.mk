
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

MYCFLAGS = -DTIMED_TEST
APP=timed_test

include $(TOP)/mkfiles/test.mk
