
TOP = ..
SOURCE=\
	nop.cpp\
	monitor.cpp\
	force_interface.cpp\
	force.cpp\
	drivebase.cpp\
	motor_check.cpp\
	main.cpp\
	log.cpp\
	toplevel.cpp\
	gear_collector.cpp\
	gear_grabber.cpp\
	gear_lifter.cpp\
	gear_shifter.cpp\
	roller_arm.cpp\
	roller.cpp\
	sim.cpp\
	shooter.cpp\
	climber.cpp\
	pump.cpp\
	lights.cpp

MYLIBS=\
	executive\
	input\
	util\
	pixycam

MYCFLAGS=-DMONITOR_TEST
APP=monitor_test

include $(TOP)/mkfiles/test.mk
