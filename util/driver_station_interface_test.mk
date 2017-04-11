
TOP = ..
SOURCE=\
	driver_station_interface.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DDRIVER_STATION_INTERFACE_TEST
APP=driver_station_interface

include $(TOP)/mkfiles/test.mk
