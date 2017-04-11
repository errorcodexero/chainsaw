
TOP = ..
SOURCE=\
	string_utils.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DSTRING_UTILS_TEST
APP=string_utils

include $(TOP)/mkfiles/test.mk
