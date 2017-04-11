
TOP = ..
SOURCE=\
	nop.cpp\
	force.cpp\
	force_interface.cpp\
	toplevel.cpp\
	drivebase.cpp\
	pump.cpp\
	monitor.cpp\
	motor_check.cpp\
	log.cpp\
	gear_collector.cpp\
	gear_grabber.cpp\
	gear_lifter.cpp\
	gear_shifter.cpp\
	roller_arm.cpp\
	roller.cpp\
	sim.cpp\
	shooter.cpp\
	climber.cpp\
	lights.cpp\
	main.cpp


MYLIBS=\
	executive\
	input\
	util\
	pixycam

MYCFLAGS=-DMAIN_TEST
APP=main_test

include $(TOP)/mkfiles/test.mk
