
TOP = ..
SOURCE=\
	network.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DNETWORK_TEST
APP=network

include $(TOP)/mkfiles/test.mk
