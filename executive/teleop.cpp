#include "teleop.h"
#include <math.h>
#include "delay.h"
#include "../input/util.h"

using namespace std;

double set_drive_speed(double axis,double boost,double slow){
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

Teleop::Teleop():shifter_goal(Gear_shifter::Goal::LOW){}

Toplevel::Goal Teleop::run(Run_info info) {
	Toplevel::Goal goals;
	
	bool enabled = info.in.robot_mode.enabled;

	{//Set drive goals
		bool spin=fabs(info.main_joystick.axis[Gamepad_axis::RIGHTX])>.01;//drive turning button
		double boost=info.main_joystick.axis[Gamepad_axis::LTRIGGER],slow=info.main_joystick.axis[Gamepad_axis::RTRIGGER];//turbo and slow buttons	

		POV_section driver_pov=pov_section(info.main_joystick.pov);
		
		const array<POV_section,POV_NUDGES> nudge_povs={POV_section::UP,POV_section::DOWN,POV_section::LEFT,POV_section::RIGHT};
		const array<unsigned int,BUTTON_NUDGES> nudge_buttons={Gamepad_button::LB,Gamepad_button::RB};
		//Forward, backward, left, right, clockwise, counter-clockwise
		for(unsigned i=0;i<NUDGES;i++){
			bool nudge_button_pressed=i<POV_NUDGES?driver_pov==nudge_povs[i]:info.main_joystick.button[nudge_buttons[i%POV_NUDGES]];
			if(nudges[i].trigger(boost<.25 && nudge_button_pressed)) nudges[i].timer.set(.1);
			nudges[i].timer.update(info.in.now,enabled);
		}
		const double NUDGE_POWER=.2,ROTATE_NUDGE_POWER=.5;
		goals.drive.left=([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return -NUDGE_POWER;
			if(!nudges[Nudges::BACKWARD].timer.done()) return NUDGE_POWER;
			if(!nudges[Nudges::CLOCKWISE].timer.done()) return -ROTATE_NUDGE_POWER;
			if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return ROTATE_NUDGE_POWER;
			double power=set_drive_speed(info.main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power+=set_drive_speed(-info.main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return power;
		}());
		goals.drive.right=clip([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return -NUDGE_POWER;
			if(!nudges[Nudges::BACKWARD].timer.done()) return NUDGE_POWER;
			if(!nudges[Nudges::CLOCKWISE].timer.done()) return -ROTATE_NUDGE_POWER;	
			if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return ROTATE_NUDGE_POWER;
			double power=set_drive_speed(info.main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power-=set_drive_speed(-info.main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return power;
		}());
	}

	if(info.main_joystick.button[Gamepad_button::B]) shifter_goal=Gear_shifter::Goal::AUTO;
	if(info.main_joystick.button[Gamepad_button::A]) shifter_goal=Gear_shifter::Goal::LOW;
	if(info.main_joystick.button[Gamepad_button::Y]) shifter_goal=Gear_shifter::Goal::HIGH;
	goals.shifter=shifter_goal;

	return goals;
}

#define TELEOP_ITEMS_NO_TYPE(X)\
	X(nudges)\
	X(shifter_goal)

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
