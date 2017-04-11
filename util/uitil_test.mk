
TOP = ..
SOURCE=\
	uitil.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DUITIL_TEST
APP=uitil

include $(TOP)/mkfiles/test.mk
