
TOP = ..
SOURCE=\
	input.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DINPUT_TEST
APP=input_test

include $(TOP)/mkfiles/test.mk
