#include "auto_gearloading.h"
#include "teleop.h"

using namespace std;

Executive Auto_gearloading_topeg::next_mode(Next_mode_info /*info*/){
	nyi/*pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.l-initial_encoders.first,info.status.drive.ticks.r-initial_encoders.second);
	Drivebase::Encoder_ticks encoder_differences = info.status.drive.ticks-initial_encoders;
	if(!info.autonomous) return Executive{Teleop()};
	const double TARGET_DISTANCE = 5.0*12.0;//inches
	const double TOLERANCE = 6.0;//inches
	motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.l<<"   "<<ticks_to_inches(encoder_differences.l)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.l) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.l) <= TARGET_DISTANCE+TOLERANCE){
		in_auto_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_auto_range.set(2.0);
	}
	if(in_auto_range.done()){
		//set_initial_encoders=false;
		return Executive{Auto_gearloading_turn(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARLOADING_TURN_ITEMS))};
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_gearloading_topeg(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARLOADING_TOPEG_ITEMS))};*/
}

Toplevel::Goal Auto_gearloading_topeg::run(Run_info /*info*/){
	Toplevel::Goal goals;
	nyi/*double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));
	goals.drive.left=power;
	goals.drive.right=power;
	return goals;*/
}

Executive Auto_gearloading_turn::next_mode(Next_mode_info /*info*/){
	nyi/*pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.l-initial_encoders.first,info.status.drive.ticks.r-initial_encoders.second);
	Drivebase::Encoder_ticks encoder_differences=info.status.drive.ticks-initial_encoders-initial_encoders;
	if(!info.autonomous) return Executive{Teleop()};	
	const double TARGET_DISTANCE = 12.0;//inches
	const double TOLERANCE = .1;//inches
	motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.l<<"   "<<ticks_to_inches(encoder_differences.l)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.l) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.l) <= TARGET_DISTANCE+TOLERANCE){
		in_auto_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_auto_range.set(2.0);
	}
	if(in_auto_range.done()){
		//set_initial_encoders=false;
		return Executive{Auto_gearloading_approach(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARLOADING_APPROACH_ITEMS))};	
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_gearloading_turn(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARLOADING_TURN_ITEMS))};*/
}

Toplevel::Goal Auto_gearloading_turn::run(Run_info /*info*/){
	Toplevel::Goal goals;
	nyi/*double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));
	goals.drive.left=-power;
	goals.drive.right=power;
	return goals;*/
}

Executive Auto_gearloading_approach::next_mode(Next_mode_info /*info*/){
	nyi/*pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.l-initial_encoders.first,info.status.drive.ticks.r-initial_encoders.second);
	Drivebase::Encoder_ticks encoder_differences=info.status.drive.ticks-initial_encoders-initial_encoders;
	if(!info.autonomous) return Executive{Teleop()};
	const double TARGET_DISTANCE = 12.0;//inches
	const double TOLERANCE = 6;//inches
	motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.l<<"   "<<ticks_to_inches(encoder_differences.l)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.l) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.l) <= TARGET_DISTANCE+TOLERANCE){
		in_auto_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_auto_range.set(2.0);
	}
	if(in_auto_range.done()){
		//set_initial_encoders=false;  
		return Executive{Auto_gearloading_geardrop(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARLOADING_ITEMS))}; 
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_gearloading_approach(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARLOADING_APPROACH_ITEMS))};*/
}

Toplevel::Goal Auto_gearloading_approach::run(Run_info /*info*/){
	Toplevel::Goal goals;
	nyi/*double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));
	goals.drive.left=-power;
	goals.drive.right=power;
	return goals;*/
}

Executive Auto_gearloading_geardrop::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};

	return Executive{Auto_gearloading_geardrop(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARLOADING_ITEMS))};
}

Toplevel::Goal Auto_gearloading_geardrop::run(Run_info){
	return {};
}


#define STEPS \
	X(topeg) \
	X(turn) \
	X(approach) \
	X(geardrop)

#define X(NAME) bool Auto_gearloading_##NAME::operator==(Auto_gearloading_##NAME const& a)const{ return gear_step==a.gear_step; }
STEPS
#undef X

#ifdef AUTO_GEARLOADING_TEST
#include "test.h"
int main(){
	/*#define X(NAME) { Auto_gearloading_##NAME a(0,{0,0}); test_executive(a); }
	STEPS
	#undef X*/
}
#endif

