/*
#include "auto_gearmid.h"
#include "teleop.h"

using namespace std;

Executive Auto_gearmid_topeg::next_mode(Next_mode_info info){
i	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.l-initial_encoders.first,info.status.drive.ticks.r-initial_encoders.second);
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
		return Executive{Teleop()};
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_gearmid_topeg(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARMID_TOPEG_ITEMS))};
}

Executive Auto_gearmid_geardrop::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};

	return Executive{Auto_gearmid_geardrop(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARMID_ITEMS))};
}

Toplevel::Goal Auto_gearmid_geardrop::run(Run_info){
	return {};
}


#define STEPS \
	X(topeg) \
	X(geardrop)

#define X(NAME) bool Auto_gearmid_##NAME::operator==(Auto_gearmid_##NAME const& a)const{ return gear_step==a.gear_step; }
STEPS
#undef X

#ifdef AUTO_GEARMID_TEST
#include "test.h"
int main(){
	#define X(NAME) { Auto_gearmid_##NAME a(0,std::make_pair(0,0)); test_executive(a); }
	STEPS
	#undef X
}
#endif
*/
