
TOP = ..
SOURCE=\
	meta.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DMETA_TEST
APP=meta

include $(TOP)/mkfiles/test.mk
