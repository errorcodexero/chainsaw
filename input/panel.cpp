#include "panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>

using namespace std;
static const unsigned int BALL_COL_AXIS=0,BELT_DIR_AXIS=1,BALL_ARM_AXIS=2,BALL_SHOOTER_AXIS=3,GEAR_HOLDER_AXIS=4,GEAR_COL_AXIS=5,AUTO_SEL_AXIS=6,SHOOTER_DIAL_AXIS=7;//TODO: rename constants
static const unsigned int BALL_COL_LOC=0,PREP_SHOOT_LOC=1,SHOOT_LOC=2,COL_GEAR_LOC=3,PREP_GEAR_LOC=4,PREP_SCORE_GEAR_LOC=5,SCORE_GEAR_LOC=6,CLIMBER_LOC=7,LEARN_LOC=8;//TODO: rename constants
static const unsigned int LIFT_LOC_1 = 9, LIFT_LOC_2 = 10;

#define BUTTONS \
	X(learn) X(collect) X(shoot_prep) X(shoot) X(gear_collect) X(gear_prep_collect) X(gear_prep_score) X(gear_score) X(climb)
#define THREE_POS_SWITCHES \
	X(gear_lifter) X(gear_grabber) X(shooter) X(arm) X(intake_direction) X(intake_control) X(lift)
#define TEN_POS_SWITCHES \
	X(auto_select)

#define DIALS 

#define PANEL_ITEMS \
	BUTTONS \
	THREE_POS_SWITCHES \
	TEN_POS_SWITCHES \
	DIALS


Panel::Panel():
	in_use(0),
	#define X(BUTTON) BUTTON(false),
	BUTTONS
	#undef X
	gear_lifter(Gear_lifter::UP),
	gear_grabber(Gear_grabber::OPEN),
	shooter(Shooter::OFF),
	arm(Arm::STOW),
	intake_direction(Intake_direction::IN),
	intake_control(Intake_control::OFF),
	lift(Lift::UP),
	auto_select(0)
{}

ostream& operator<<(ostream& o,Panel::Gear_lifter p){
	#define X(NAME) if(p==Panel::Gear_lifter::NAME) return o<<""#NAME;
	X(UP) X(DOWN) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Lift l){
	#define X(STATE) if(l==Panel::Lift::STATE) return o<<""#STATE;
	X(UP) X(DOWN) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Gear_grabber p){
	#define X(NAME) if(p==Panel::Gear_grabber::NAME) return o<<""#NAME;
	X(OPEN) X(CLOSE) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Shooter p){
	#define X(NAME) if(p==Panel::Shooter::NAME) return o<<""#NAME;
	X(ON) X(OFF) X(REVERSE)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Arm p){
	#define X(NAME) if(p==Panel::Arm::NAME) return o<<""#NAME;
	X(LOW) X(STOW) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Intake_direction p){
	#define X(NAME) if(p==Panel::Intake_direction::NAME) return o<<""#NAME;
	X(IN) X(OUT) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Intake_control p){
	#define X(NAME) if(p==Panel::Intake_control::NAME) return o<<""#NAME;
	X(ON) X(OFF) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel p){
	o<<"Panel(";
	o<<"in_use:"<<p.in_use;
	#define X(NAME) o<<", "#NAME":"<<p.NAME;
	PANEL_ITEMS
	#undef X
	return o<<")";
}

bool operator==(Panel const& a,Panel const& b){
	return true
	#define X(NAME) && a.NAME==b.NAME
	PANEL_ITEMS
	#undef X
	;
}

bool operator!=(Panel const& a,Panel const& b){
	return !(a==b);
}

float axis_to_percent(double a){
	return .5-(a/2);
}

bool set_button(const float AXIS_VALUE, const float LOWER_VALUE, const float TESTING_VALUE, const float UPPER_VALUE){
	float lower_tolerance = (TESTING_VALUE - LOWER_VALUE)/2;
	float upper_tolerance = (UPPER_VALUE - TESTING_VALUE)/2;
	float min = TESTING_VALUE - lower_tolerance;
	float max = TESTING_VALUE + upper_tolerance; 
	return (AXIS_VALUE > min && AXIS_VALUE < max);
}

bool get_in_use(Joystick_data d){
	for(int i = 0; i < JOY_AXES; i++) {
		if(d.axis[i] != 0) return true;
	}
	for(int i = 0; i < JOY_BUTTONS; i++) {
		if(d.button[i] != 0) return true;
	}
	return false;
}

Panel interpret_oi(Joystick_data d){
	Panel p;
	static const float ARTIFICIAL_MAX = 1.5;
	{
		p.in_use=get_in_use(d);
		if(!p.in_use) return p;
	}
	{//set the auto mode number from the dial value
		float auto_dial_value = d.axis[AUTO_SEL_AXIS];
		p.auto_select = interpret_10_turn_pot(auto_dial_value);
	}
	{//two position switches
	}
	{//three position switches
		float gear_lifter = d.axis[GEAR_COL_AXIS];
		p.gear_lifter = [&]{
			static const float UP = -1, DOWN = 0, AUTO = 1;
			if(set_button(gear_lifter,UP,DOWN,AUTO)) return Panel::Gear_lifter::DOWN;
			if(set_button(gear_lifter,DOWN,AUTO,ARTIFICIAL_MAX)) return Panel::Gear_lifter::AUTO;
			return Panel::Gear_lifter::UP;
		}();

		float gear_grabber = d.axis[GEAR_HOLDER_AXIS];
		p.gear_grabber = [&]{
			static const float OPEN = -1, CLOSE = 0, AUTO = 1;
			if(set_button(gear_grabber,OPEN,CLOSE,AUTO)) return Panel::Gear_grabber::CLOSE;
			if(set_button(gear_grabber,CLOSE,AUTO,ARTIFICIAL_MAX)) return Panel::Gear_grabber::AUTO;
			return Panel::Gear_grabber::OPEN;
		}();	
	 
		float shooter = d.axis[BALL_SHOOTER_AXIS];
		p.shooter = [&]{
			static const float ON = -1, OFF = 0, REVERSE = 1;
			if(set_button(shooter,ON,OFF,REVERSE)) return Panel::Shooter::OFF;
			if(set_button(shooter,OFF,REVERSE,ARTIFICIAL_MAX)) return Panel::Shooter::REVERSE;
			return Panel::Shooter::ON;
		}();
		
		
		float arm = d.axis[BALL_ARM_AXIS];
		p.arm = [&]{
			static const float STOW = -1, LOW = 0, AUTO = 1;
			if(set_button(arm,STOW,LOW,AUTO)) return Panel::Arm::LOW;
			if(set_button(arm,LOW,AUTO,ARTIFICIAL_MAX)) return Panel::Arm::AUTO;
			return Panel::Arm::STOW;
		}();
			
		float intake_direction = d.axis[BELT_DIR_AXIS];
		p.intake_direction = [&]{
			static const float IN = -1, OUT = 0, AUTO = 1;
			if(set_button(intake_direction,IN,OUT,AUTO)) return Panel::Intake_direction::OUT;
			if(set_button(intake_direction,IN,AUTO,ARTIFICIAL_MAX)) return Panel::Intake_direction::AUTO;
			return Panel::Intake_direction::IN;
		}();

		float intake_control = d.axis[BALL_COL_AXIS];
		p.intake_control = [&]{
			static const float ON = -1, OFF = 0, AUTO = 1;
			if(set_button(intake_control,ON,OFF,AUTO)) return Panel::Intake_control::OFF;
			if(set_button(intake_control,OFF,AUTO,ARTIFICIAL_MAX)) return Panel::Intake_control::AUTO;
			return Panel::Intake_control::ON;
		}();	
		{
			bool lift_1 = d.button[LIFT_LOC_1], lift_2 = d.button[LIFT_LOC_2];
			p.lift = [&]{
				if(lift_1) return Panel::Lift::UP;
				if(lift_2) return Panel::Lift::DOWN;
				return Panel::Lift::AUTO;
			}();
		}
	}
	{//buttons
		/*//sets all buttons to off beacuse we assume that only one should be pressed on this axis at a time
		#define X(button) p.button = false;
		BUTTONS
		#undef X*/
		p.learn=d.button[LEARN_LOC];
		p.collect=d.button[BALL_COL_LOC];
		p.shoot_prep=d.button[PREP_SHOOT_LOC];
		p.shoot=d.button[SHOOT_LOC];
		p.gear_collect=d.button[COL_GEAR_LOC];
		p.gear_prep_collect=d.button[PREP_GEAR_LOC];
		p.gear_prep_score=d.button[PREP_SCORE_GEAR_LOC];
		p.gear_score=d.button[SCORE_GEAR_LOC];
		p.climb=d.button[CLIMBER_LOC];
		
		//const float AXIS_VALUE = d.axis[2];
		//static const float DEFAULT=-1, ARTIFICIAL_MAX = 1.38;
	
		//old replaced by set_button:
		/*#define AXIS_RANGE(axis, last, curr, next, var, val) if (axis > curr-(curr-last)/2 && axis < curr+(next-curr)/2) var = val;
		AXIS_RANGE(op, DEFAULT, GRABBER_OPEN, GRABBER_CLOSE, p.grabber_open, 1)
		else AXIS_RANGE(op, GRABBER_OPEN, GRABBER_CLOSE, PREP, p.grabber_close, 1)
		else AXIS_RANGE(op, GRABBER_CLOSE, PREP, SHOOT, p.prep, 1)
		else AXIS_RANGE(op, PREP, SHOOT, 1.38, p.shoot, 1)
		#undef AXIS_RANGE*/
		
	}
	{//Dials
		p.speed_dial = -d.axis[SHOOTER_DIAL_AXIS];
	}
	return p;
}

Panel interpret_test_oi(Joystick_data d){	
	//static const unsigned int POTENTIOMETER_AXIS=1, TEN_POS_SWITCH_AXIS = 0; //TODO: need real values
	//static const unsigned int BUTTON_0_LOC=0, BUTTON_1_LOC=1, BUTTON_2_LOC=2, BUTTON_3_LOC=3, TWO_POS_SWITCH_0_LOC = 4, TWO_POS_SWITCH_1_LOC =5; //TODO: need real values

	Panel p;
	{
		p.in_use=[&](){
			for(int i=0;i<JOY_AXES;i++) {
				if(d.axis[i]!=0)return true;
			}
			for(int i=0;i<JOY_BUTTONS;i++) {
				if(d.button[i]!=0)return true;
			}
			return false;
		}();
		if(!p.in_use) return p;
	}
	//p. = interpret_10_turn_pot(d.axis[TEN_POS_SWITCH_AXIS]); //set the switch value from the pot value
	{//two position switches
		//p. = d.button[TWO_POS_SWITCH_0_LOC];
		//p. = d.button[TWO_POS_SWITCH_1_LOC];
	}
	{//buttons
		//p. = d.button[BUTTON_0_LOC];
		//p. = d.button[BUTTON_1_LOC];
		//p. = d.button[BUTTON_2_LOC];
		//p. = d.button[BUTTON_3_LOC];
	}
	{//dials
		//p. = d.axis[POTENTIOMETER_AXIS];
	}
	return p;
}

Panel interpret_gamepad(Joystick_data d){
	Panel p;
	p.in_use = get_in_use(d);
	if(!p.in_use) return p;
	
	bool alternative_op = d.button[Gamepad_button::LB];

	if(!alternative_op){
		p.learn = d.button[Gamepad_button::BACK];
		p.climb = d.button[Gamepad_button::START];
		p.gear_score = d.button[Gamepad_button::Y];
		p.gear_prep_collect = d.button[Gamepad_button::A];
		p.speed_dial = d.axis[Gamepad_axis::LTRIGGER];
		switch(pov_section(d.axis[Gamepad_axis::DPAD])){
			case POV_section::CENTER:
				break;
			case POV_section::UP:
				p.arm = Panel::Arm::STOW;
				break;
			case POV_section::UP_LEFT:
				break;
			case POV_section::LEFT:
				p.arm = Panel::Arm::AUTO;
				break;
			case POV_section::DOWN_LEFT:
				break;
			case POV_section::DOWN:
				p.arm = Panel::Arm::LOW;
				break;
			case POV_section::DOWN_RIGHT:
				break;
			case POV_section::RIGHT:
				break;
			case POV_section::UP_RIGHT:
				break;
			default:
				assert(0);
		}
		switch(joystick_section(d.axis[Gamepad_axis::LEFTX],d.axis[Gamepad_axis::LEFTY])){
			case Joystick_section::UP:
				p.gear_lifter = Panel::Gear_lifter::UP;
				break;
			case Joystick_section::LEFT:
				p.gear_lifter = Panel::Gear_lifter::AUTO;
				break;
			case Joystick_section::DOWN:
				p.gear_lifter = Panel::Gear_lifter::DOWN;
				break;
			case Joystick_section::RIGHT:
				break;
			case Joystick_section::CENTER:
				break;
			default:
				assert(0);
		}
		switch(joystick_section(d.axis[Gamepad_axis::RIGHTX],d.axis[Gamepad_axis::RIGHTY])){
			case Joystick_section::UP:
			 	p.intake_direction = Panel::Intake_direction::IN;
					break;
			case Joystick_section::LEFT:
				p.intake_direction = Panel::Intake_direction::AUTO;
					break;
			case Joystick_section::DOWN:
				p.intake_direction = Panel::Intake_direction::OUT;
					break;
			case Joystick_section::RIGHT:
				break;
			case Joystick_section::CENTER:
				break;
			default:
				assert(0);
		}
	} else{
		p.shoot = d.button[Gamepad_button::B];
		p.shoot_prep = d.button[Gamepad_button::X];
		switch(pov_section(d.axis[Gamepad_axis::DPAD])){
			case POV_section::CENTER:
				break;
			case POV_section::UP:
				p.shooter = Panel::Shooter::ON;
				break;
			case POV_section::UP_LEFT:
				break;
			case POV_section::LEFT:
				p.shooter = Panel::Shooter::REVERSE;
				break;
			case POV_section::DOWN_LEFT:
				break;
			case POV_section::DOWN:
				p.shooter = Panel::Shooter::OFF;
				break;
			case POV_section::DOWN_RIGHT:
				break;
			case POV_section::RIGHT:
				break;
			case POV_section::UP_RIGHT:
				break;
			default:
				assert(0);
		}
		switch(joystick_section(d.axis[Gamepad_axis::LEFTX],d.axis[Gamepad_axis::LEFTY])){
			case Joystick_section::UP:
				p.gear_grabber = Panel::Gear_grabber::OPEN;
				break;
			case Joystick_section::LEFT:
				p.gear_grabber = Panel::Gear_grabber::AUTO;
				break;
			case Joystick_section::DOWN:
				p.gear_grabber = Panel::Gear_grabber::CLOSE;
				break;
			case Joystick_section::RIGHT:
				break;
			case Joystick_section::CENTER:
				break;
			default:
				assert(0);
		}
		switch(joystick_section(d.axis[Gamepad_axis::RIGHTX],d.axis[Gamepad_axis::RIGHTY])){
			case Joystick_section::UP:
				p.intake_control = Panel::Intake_control::ON;
				break;
			case Joystick_section::LEFT:
				p.intake_control = Panel::Intake_control::AUTO;
				break;
			case Joystick_section::DOWN:
				p.intake_control = Panel::Intake_control::OFF;
				break;
			case Joystick_section::RIGHT:
				break;
			case Joystick_section::CENTER:
				break;
			default:
				assert(0);
		}
	}
	return p;
}

Joystick_data driver_station_input_rand(){
	Joystick_data r;
	for(unsigned i=0;i<JOY_AXES;i++){
		r.axis[i]=(0.0+rand()%101)/100;
	}
	for(unsigned i=0;i<JOY_BUTTONS;i++){
		r.button[i]=rand()%2;
	}
	return r;
}

Panel rand(Panel*){
	return interpret_oi(driver_station_input_rand());
}

#ifdef PANEL_TEST
int main(){
	Panel p;
	for(unsigned i=0;i<50;i++){
		interpret_oi(driver_station_input_rand());
	}
	cout<<p<<"\n";
	return 0;
}
#endif
