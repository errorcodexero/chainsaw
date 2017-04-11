
TOP = ..
SOURCE=\
	nop.cpp\
	log.cpp\
	toplevel.cpp\
	drivebase.cpp\
	pump.cpp\
	motor_check.cpp\
	climber.cpp\
	gear_collector.cpp\
	gear_grabber.cpp\
	gear_lifter.cpp\
	gear_shifter.cpp\
	roller_arm.cpp\
	roller.cpp\
	sim.cpp\
	shooter.cpp\
	lights.cpp

MYLIBS=\
	util\
	pixycam

MYCFLAGS=-DLOG_TEST
APP=log_test

include $(TOP)/mkfiles/test.mk
