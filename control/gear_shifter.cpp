#include "gear_shifter.h"

using namespace std;

#define R_SHIFTER_SOLENOID 0
#define L_SHIFTER_SOLENOID 1

Robot_outputs Gear_shifter::Output_applicator::operator(Robot_outputs robot,Output out){
	robot.solenoid[R_SHIFTER_SOLENOID]=out==Output::HIGH;
	robot.solenoid[L_SHIFTER_SOLENOID]=out==Output::HIGH;
	return robot;
}

Gear_shifter::Output Gear_shifter::Output_applicator::operator(Robot_outputs robot){
	//Assumes that both solenoids are set to the same value
	return robot.solenoid[R_SHIFTER_SOLENOID]?Output::HIGH:Output::LOW;
}



set<Gear_shifter::Goal> examples(Gear_shifter::Goal*){
	#define X(A) Gear_shifter::Goal::A,
	return {GEAR_SHIFTER_GOALS};
}

set<Gear_shifter::Output> examples(Gear_shifter::Output*){
	#define X(A) Gear_shifter::Output::A,
	return {GEAR_SHIFTER_OUTPUTS};
}

Gear_shifter::Status status(Status_detail status){
	return {};
}

Gear_shifter::Output control(Gear_shifter::Status_detail status,Gear_shifter::Goal goal){
	switch(goal) {
		case Gear_shifter::Goal::AUTO: return status;
		case Gear_shifter::Goal::LOW: return Gear_shifter::Output::LOW;
		case Gear_shifter::Goal::HIGH: return Gear_shifter::Output::HIGH;
		default: assert(0);
	}
}

bool ready(Drivebase::Status,Drivebase::Goal){
	return 1;
}
