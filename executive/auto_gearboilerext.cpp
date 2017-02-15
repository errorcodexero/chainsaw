#include "auto_gearboilerext.h"
#include "teleop.h"

using namespace std;

Executive Auto_gearboilerext_frompeg::next_mode(Next_mode_info info){
	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.l-initial_encoders.first,info.status.drive.ticks.r-initial_encoders.second);
	if(!info.autonomous) return Executive{Teleop()};
	const double TARGET_DISTANCE = 5.0*12.0;//inches
	const double TOLERANCE = 6.0;//inches
	motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.first<<"   "<<ticks_to_inches(encoder_differences.first)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.first) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.first) <= TARGET_DISTANCE+TOLERANCE){
		in_auto_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_auto_range.set(2.0);
	}
	if(in_auto_range.done()){
		//set_initial_encoders=false;
		return Executive{Auto_gearboilerext_turn(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_TURN_ITEMS))};
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_gearboilerext_frompeg(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_FROMPEG_ITEMS))};
}

Toplevel::Goal Auto_gearboilerext_frompeg::run(Run_info info){
	Toplevel::Goal goals;
	double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));
	goals.drive.left=power;
	goals.drive.right=power;
	return goals;
}

Executive Auto_gearboilerext_turn::next_mode(Next_mode_info info){
	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.l-initial_encoders.first,info.status.drive.ticks.r-initial_encoders.second);
	if(!info.autonomous) return Executive{Teleop()};	
	const double TARGET_DISTANCE = 12.0;//inches
	const double TOLERANCE = .1;//inches
	motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.first<<"   "<<ticks_to_inches(encoder_differences.first)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.first) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.first) <= TARGET_DISTANCE+TOLERANCE){
		in_auto_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_auto_range.set(2.0);
	}
	if(in_auto_range.done()){
		//set_initial_encoders=false;
		return Executive{Auto_gearboilerext_driveone(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_DRIVEONE_ITEMS))};	
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_gearboilerext_turn(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_TURN_ITEMS))};
}

Toplevel::Goal Auto_gearboilerext_turn::run(Run_info info){
	Toplevel::Goal goals;
	double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));
	goals.drive.left=-power;
	goals.drive.right=power;
	return goals;
}

Executive Auto_gearboilerext_driveone::next_mode(Next_mode_info info){
	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.l-initial_encoders.first,info.status.drive.ticks.r-initial_encoders.second);
	if(!info.autonomous) return Executive{Teleop()};
	const double TARGET_DISTANCE = 12.0;//inches
	const double TOLERANCE = 6;//inches
	motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.first<<"   "<<ticks_to_inches(encoder_differences.first)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.first) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.first) <= TARGET_DISTANCE+TOLERANCE){
		in_auto_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_auto_range.set(2.0);
	}
	if(in_auto_range.done()){
		//set_initial_encoders=false;  
		return Executive{Auto_gearboilerext_geardrop(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_ITEMS))}; 
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_gearboilerext_driveone(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_DRIVEONE_ITEMS))};
}

Toplevel::Goal Auto_gearboilerext_driveone::run(Run_info info){
	Toplevel::Goal goals;
	double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));
	goals.drive.left=-power;
	goals.drive.right=power;
	return goals;
}

Executive Auto_gearboilerext_turntwo::next_mode(Next_mode_info info){
	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.l-initial_encoders.first,info.status.drive.ticks.r-initial_encoders.second);
	if(!info.autonomous) return Executive{Teleop()};
	const double TARGET_DISTANCE = 12.0;//inches
	const double TOLERANCE = 6;//inches
	motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.first<<"   "<<ticks_to_inches(encoder_differences.first)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.first) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.first) <= TARGET_DISTANCE+TOLERANCE){
		in_auto_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_auto_range.set(2.0);
	}
	if(in_auto_range.done()){
		//set_initial_encoders=false;  
		return Executive{Auto_gearboilerext_geardrop(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_ITEMS))};
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_gearboilerext_turntwo(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_TURNTWO_ITEMS))};
}

Toplevel::Goal Auto_gearboilerext_turntwo::run(Run_info info){
	Toplevel::Goal goals;
	double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));
	goals.drive.left=-power;
	goals.drive.right=power;
	return goals;
}

Executive Auto_gearboilerext_drivetwo::next_mode(Next_mode_info info){
	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.l-initial_encoders.first,info.status.drive.ticks.r-initial_encoders.second);
	if(!info.autonomous) return Executive{Teleop()};
	const double TARGET_DISTANCE = 12.0;//inches
	const double TOLERANCE = 6;//inches
	motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.first<<"   "<<ticks_to_inches(encoder_differences.first)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.first) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.first) <= TARGET_DISTANCE+TOLERANCE){
	in_auto_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_auto_range.set(2.0);
	}
	if(in_auto_range.done()){	
		//set_initial_encoders=false;  
		return Executive{Auto_gearboilerext_geardrop(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_ITEMS))};
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_gearboilerext_drivetwo(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARBOILEREXT_DRIVETWO_ITEMS))};
}

Toplevel::Goal Auto_gearboilerext_drivetwo::run(Run_info info){
	Toplevel::Goal goals;
	double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));
	goals.drive.left=-power;
	goals.drive.right=power;
	return goals;
}





#define STEPS \
	X(frompeg) \
	X(turn) \
	X(driveone) \
	X(turntwo) \
	X(drivetwo)

#define X(NAME) bool Auto_gearboilerext_##NAME::operator==(Auto_gearboilerext_##NAME const& a)const{ return gear_step==a.gear_step; }
STEPS
#undef X

#ifdef AUTO_GEARBOILEREXT_TEST
#include "test.h"
int main(){
	#define X(NAME) { Auto_gearboilerext_##NAME a(0,std::make_pair(0,0)); test_executive(a); }
	STEPS
	#undef X
}
#endif

