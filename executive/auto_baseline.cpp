#include "auto_baseline.h"
#include "teleop.h"

using namespace std;

Executive Auto_baseline::next_mode(Next_mode_info info){
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
	return Executive{Auto_baseline_turn(CONSTRUCT_STRUCT_PARAMS(AUTO_BASELINE_ITEMS))};
	}
	return Executive{Auto_baseline(CONSTRUCT_STRUCT_PARAMS(AUTO_BASELINE_ITEMS))};
}

Toplevel::Goal Auto_baseline::run(Run_info info){
	Toplevel::Goal goals;
	double power = -motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));//assuming that the left and right encoder values are similar enough
	goals.drive.left = power;
	goals.drive.right = power;
		return goals;
}

/*
Executive Auto_baseline_turn::next_mode(Next_mode_info info){
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
	return Executive{Auto_baseline_turn(CONSTRUCT_STRUCT_PARAMS(AUTO_BASELINE_ITEMS))};
}

Toplevel::Goal Auto_baseline_turn::run(Run_info info){
	Toplevel::Goal goals;
	double power = -motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));//assuming that the left and right encoder values are similar enough
	goals.drive.left = power;
	goals.drive.right = power;
	return goals;
}
*/
bool Auto_baseline::operator==(Auto_baseline const&)const{ return true; }//TODO: update with values in that struct
//bool Auto_baseline::operator==(Auto_baseline_turn const&) const{ return true}

#ifdef AUTO_BASELINE_TEST
#include "test.h"

int main(){
	Auto_baseline a = {{0,0,0}};
	test_executive(a);
}
#endif

