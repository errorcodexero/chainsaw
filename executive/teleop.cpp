#include "teleop.h"
#include <math.h>
#include "delay.h"
#include "../input/util.h"

using namespace std;

double set_drive_speed(double axis,double boost,bool slow){
	static const float MAX_SPEED=1;//Change this value to change the max power the robot will achieve with full boost (cannot be larger than 1.0)
	static const float DEFAULT_SPEED=.4;//Change this value to change the default power
	static const float SLOW_BY=.5;//Change this value to change the percentage of the default power the slow button slows
	return (pow(axis,3)*((DEFAULT_SPEED+(MAX_SPEED-DEFAULT_SPEED)*boost)-((DEFAULT_SPEED*SLOW_BY)*slow)));
}

bool operator<(Teleop::Nudge const& a,Teleop::Nudge const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	NUDGE_ITEMS(X)
	#undef X
	return 0;
}

bool operator==(Teleop::Nudge const& a,Teleop::Nudge const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	NUDGE_ITEMS(X)
	#undef X
	return 1;
}

ostream& operator<<(ostream& o,Teleop::Gear_score_step const& a){
	#define X(NAME) if(a==Teleop::Gear_score_step::NAME) return o<<""#NAME;
	GEAR_SCORE_STEPS	
	#undef X
	assert(0);
}

Executive Teleop::next_mode(Next_mode_info info) {
	if (info.autonomous_start) {
		if (info.panel.in_use) {
			return Executive{Delay()};
		}
	}
	Teleop t(CONSTRUCT_STRUCT_PARAMS(TELEOP_ITEMS));
	return Executive{t};
}

IMPL_STRUCT(Teleop::Teleop,TELEOP_ITEMS)

Teleop::Teleop():gear_score_step(Gear_score_step::CLEAR_BALLS){}

void Teleop::gear_score_protocol(Toplevel::Status_detail const& status,const bool enabled,const Time now,Toplevel::Goal& goals){
	//TODO: set clear_ball_timer somewhere
	switch(gear_score_step){
		case Gear_score_step::CLEAR_BALLS:
			clear_ball_timer.update(now,enabled);
			goals.collector.intake = Intake::Goal::OUT;
			//goals.collector.lift = Lift::Goal::UP;//TODO
			goals.collector.arm = Arm::Goal::OUT;
			if(clear_ball_timer.done()) gear_score_step = Gear_score_step::RAISE_ARM;
			break;	
		case Gear_score_step::RAISE_ARM:
			goals.collector.intake = Intake::Goal::OFF;
			//goals.collector.lift = Lift::Goal::OFF;//TODO
			goals.collector.arm = Arm::Goal::IN;
			if(ready(status.collector.arm,goals.collector.arm)) gear_score_step = Gear_score_step::LIFT_GEAR;
			break;
		case Gear_score_step::LIFT_GEAR:
			goals.collector = {Intake::Goal::OFF,Arm::Goal::IN/*Lift::Goal::OFF*/};//TODO
			goals.gear_collector = {Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::UP};
			break;	
		default:
			assert(0);
	}	
}

Toplevel::Goal Teleop::run(Run_info info) {
	Toplevel::Goal goals;
	
	bool enabled = info.in.robot_mode.enabled;

	{//Set drive goals
		bool spin=fabs(info.driver_joystick.axis[Gamepad_axis::RIGHTX])>.01;//drive turning button
		double boost=info.driver_joystick.axis[Gamepad_axis::LTRIGGER]; //Turbo button
		bool slow=info.driver_joystick.button[Gamepad_button::LB]; //Slow button

		POV_section driver_pov=pov_section(info.driver_joystick.pov);
		
		const array<POV_section,NUDGES> nudge_povs={POV_section::UP,POV_section::DOWN,POV_section::LEFT,POV_section::RIGHT};
		//Forward, backward, clockwise, counter-clockwise
		for(unsigned i=0;i<NUDGES;i++){
			if(nudges[i].trigger(boost<.25 && driver_pov==nudge_povs[i])) nudges[i].timer.set(.1);
			nudges[i].timer.update(info.in.now,enabled);
		}
		const double NUDGE_POWER=.2,ROTATE_NUDGE_POWER=.2;
		goals.drive.left=([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return -NUDGE_POWER;
			if(!nudges[Nudges::BACKWARD].timer.done()) return NUDGE_POWER;
			if(!nudges[Nudges::CLOCKWISE].timer.done()) return ROTATE_NUDGE_POWER;
			if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return -ROTATE_NUDGE_POWER;
			double power=set_drive_speed(info.driver_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power+=set_drive_speed(-info.driver_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return -power; //inverted so drivebase values can be positive
		}());
		goals.drive.right=clip([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return -NUDGE_POWER;
			if(!nudges[Nudges::BACKWARD].timer.done()) return NUDGE_POWER;
			if(!nudges[Nudges::CLOCKWISE].timer.done()) return -ROTATE_NUDGE_POWER;	
			if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return ROTATE_NUDGE_POWER;
			double power=set_drive_speed(info.driver_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power-=set_drive_speed(-info.driver_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return -power; //inverted so drivebase values can be positive
		}());
	}

	goals.shifter=[&]{
		if(info.driver_joystick.button[Gamepad_button::RB]) return Gear_shifter::Goal::LOW;
		if(info.driver_joystick.axis[Gamepad_axis::RTRIGGER]>.8) return Gear_shifter::Goal::HIGH;
		return Gear_shifter::Goal::AUTO;
	}();

	//TODO: FIXME
	if(info.panel.gear_grasper==Panel::Gear_grasper::OPEN) goals.gear_collector.gear_grabber = Gear_grabber::Goal::OPEN;
	if(info.panel.gear_grasper==Panel::Gear_grasper::CLOSED) goals.gear_collector.gear_grabber = Gear_grabber::Goal::CLOSE;

	if(info.panel.gear_collector==Panel::Gear_collector::UP) goals.gear_collector.gear_lifter = Gear_lifter::Goal::UP;
	if(info.panel.gear_collector==Panel::Gear_collector::DOWN) goals.gear_collector.gear_lifter = Gear_lifter::Goal::DOWN;	

	if(info.panel.gear_prep_score){
		gear_score_protocol(info.toplevel_status,info.in.robot_mode.enabled,info.in.now,goals);
	} else if(info.panel.gear_score){
		goals.gear_collector.gear_grabber = Gear_grabber::Goal::OPEN;
	}
	if(info.panel.climb){
		goals.climber = Climber::Goal::CLIMB;
	}
	
	return goals;
}

#define TELEOP_ITEMS_NO_TYPE(X)\
	X(nudges)

bool Teleop::operator<(Teleop const& a)const{
	#define X(NAME) if(NAME<a.NAME) return 1; if(a.NAME<NAME) return 0;
	TELEOP_ITEMS_NO_TYPE(X)
	#undef X
	return 0;
}

bool Teleop::operator==(Teleop const& a)const{
	#define X(NAME) if(NAME!=a.NAME) return 0;
	TELEOP_ITEMS_NO_TYPE(X)
	#undef X
	return 1;
}

void Teleop::display(ostream& o)const{
	o<<"Teleop( ";
	#define X(NAME) o<<""#NAME<<":"<<(NAME)<<" ";
	//TELEOP_ITEMS_NO_TYPE(X)
	#undef X
	o<<")";
}

#ifdef TELEOP_TEST
#include "test.h"

int main() {
	Teleop a;
	test_executive(a);
}
#endif
