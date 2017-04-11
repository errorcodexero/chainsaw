
TOP = ..
SOURCE=\
	sim.cpp\
	drivebase.cpp\
	gear_lifter.cpp\
	gear_grabber.cpp\
	gear_collector.cpp\
	gear_shifter.cpp\
	lights.cpp\
	roller.cpp\
	roller_arm.cpp\
	pump.cpp\
	toplevel.cpp\
	climber.cpp\
	shooter.cpp\
	main.cpp\
	force_interface.cpp\
	force.cpp\
	motor_check.cpp\
	log.cpp\
	nop.cpp

MYLIBS=\
	executive\
	input\
	util\
	pixycam

MYCFLAGS=-DSIM_TEST -Wno-unused-parameter
APP=sim_test

include $(TOP)/mkfiles/test.mk
