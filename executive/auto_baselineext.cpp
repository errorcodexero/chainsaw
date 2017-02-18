#include "auto_baselineext.h"
#include "teleop.h"

using namespace std;

Executive Auto_baselineext::next_mode(Next_mode_info info){
	nyi/*
	if(!info.autonomous) return Executive{Teleop()};
	Drivebase::Encoder_ticks encoder_differences={
	#define X(ENCODER) info.status.drive.ticks.ENCODER - initial_encoders.ENCODER
		X(l),
		X(r),
	};

	const double TARGET_DISTANCE = 9.0*12.0;//inches
	const double TOLERANCE = 3.0;//inches

	motion_profile.set_goal(TARGET_DISTANCE);

	cout<<"\n" << "encoder_differences:"<< encoder_differences<<"   left(inches):"<<ticks_to_inches(encoder_differences.l)<<"   target(inches):"<<TARGET_DISTANCE<<"\n";
	
	if(ticks_to_inches(encoder_differences.l) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.l) <= TARGET_DISTANCE+TOLERANCE){
	in_range.update(info.in.now,info.in.robot_mode.enabled);
	} else {
	const double IN_RANGE_TIME = 2.0;//seconds - the time that the robot needs to be within a certain distance from the target
	in_range.set(IN_RANGE_TIME);
	}
	if(in_range.done()){
	return Executive{Teleop()};
	}
	return Executive{Auto_baselineext(CONSTRUCT_STRUCT_PARAMS(AUTO_BASELINEEXT_ITEMS))};*/
}

Toplevel::Goal Auto_baselineext::run(Run_info info){
	Toplevel::Goal goals;
	nyi/*double power = -motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));//assuming that the left and right encoder values are similar enough
	goals.drive.left = power;
	goals.drive.right = power;
		return goals;*/
}

bool Auto_baselineext::operator==(Auto_baselineext const&)const{ return true; }//TODO: update with values in that struct

#ifdef AUTO_BASELINEEXT_TEST
#include "test.h"

int main(){
	Auto_baselineext a = {{0,0,0}};
	test_executive(a);
}
#endif

