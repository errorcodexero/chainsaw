#include "auto_gearmid.h"
#include "teleop.h"

using namespace std;

Auto_gearmid_topeg::Auto_gearmid_topeg():gear_step(0),initial_encoders({0,0}),encoderflag(0){}


void Auto_gearmid_topeg::setencoderflag(Drivebase::Encoder_ticks ticks){
	if(!encoderflag){
		encoderflag=true;
		initial_encoders=ticks;
	}

}
double Auto_gearmid_topeg::distance_left(Drivebase::Encoder_ticks ticks){
	const double goal= 6.0*12.0; //inches
	int encoder_diff = ticks.l-initial_encoders.l;
	double current = ticks_to_inches(encoder_diff);
	return goal-current;
}	

Executive Auto_gearmid_topeg::next_mode(Next_mode_info info){
	setencoderflag(info.status.drive.ticks);
	//Drivebase::Encoder_ticks encoder_differences={info.status.drive.ticks.l-initial_encoders.l,info.status.drive.ticks.r-initial_encoders.r};
	if(!info.autonomous) return Executive{Teleop()};
	const double TOLERANCE = 6.0;//inches
	//motion_profile.set_goal(TARGET_DISTANCE);
	//cout<<"\n"<<encoder_differences.l<<"   "<<ticks_to_inches(encoder_differences.l)<<"\n";
	if((distance_left(info.status.drive.ticks) >= -TOLERANCE) & (distance_left(info.status.drive.ticks) <= TOLERANCE)){
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

Toplevel::Goal Auto_gearmid_topeg::run(Run_info info){
	
	Toplevel::Goal goals;
	//double power=motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.l));
	double power=clamp(distance_left(info.toplevel_status.drive.ticks)/12,-.2,.2);
	goals.drive.left=power;
	goals.drive.right=power;
	return goals;
}

Auto_gearmid_geardrop::Auto_gearmid_geardrop():gear_step(0),initial_encoders({0,0}),encoderflag(0){}

Executive Auto_gearmid_geardrop::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	if(true) return Executive{Teleop()}; //set to gear ready function when intrigrating gear manipulation
	return Executive{Auto_gearmid_geardrop(CONSTRUCT_STRUCT_PARAMS(AUTO_GEARMID_ITEMS))};
}

Toplevel::Goal Auto_gearmid_geardrop::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left=goals.drive.right=0;
	return goals;
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
	#define X(NAME) { Auto_gearmid_##NAME a(0,Drivebase::Encoder_ticks{0,0},false); test_executive(a); }
	STEPS
	#undef X
}
#endif

