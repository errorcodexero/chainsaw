
TOP = ..
SOURCE=\
	motion_profile.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DMOTION_PROFILE_TEST
APP=motion_profile

include $(TOP)/mkfiles/test.mk
