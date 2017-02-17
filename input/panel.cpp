#include "panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>

using namespace std;
static const unsigned int BALL_ARM_AXIS=0,GEAR_GRASPER_AXIS=1,GEAR_COLLECTOR_AXIS=2,SHOOTER_AXIS=3,BALL_INTAKE_AXIS=4,BALL_LIFT_AXIS=5,AUTO_SELECTOR_AXIS=6,SPEED_DIAL_AXIS=7;//TODO: rename constants
static const unsigned int BALL_COLLECT_LOC=0,LOADING_INDICATOR_LOC=1,SHOOT_LOC=2,PREP_COLLECT_GEAR_LOC=3,PREP_SCORE_GEAR_LOC=4,COLLECT_GEAR_LOC=5,SCORE_GEAR_LOC=6,CLIMB_LOC=7,LEARN_LOC=8;//TODO: rename constants
static const unsigned int SHOOTER_BELT_AUTO_LOC=9,SHOOTER_BELT_ENABLED_LOC=10,BALL_COLLECTOR_DISABLED_LOC=11,BALL_COLLECTOR_AUTO_LOC=12;

#define BUTTONS \
	X(ball_collect) X(loading_indicator) X(shoot) X(gear_prep_collect) X(gear_prep_score) X(gear_collect) X(gear_score) X(climb) X(learn)
#define THREE_POS_SWITCHES \
	X(ball_arm) X(gear_grasper) X(gear_collector) X(shooter) X(ball_intake) X(ball_lift) X(shooter_belt) X(ball_collector)
#define TEN_POS_SWITCHES \
	X(auto_select)
#define DIALS \
	X(speed_dial)

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
	ball_arm(Ball_arm::DOWN),
	gear_grasper(Gear_grasper::CLOSED),
	gear_collector(Gear_collector::DOWN),
	shooter(Shooter::DISABLED),
	ball_intake(Ball_intake::IN),
	ball_lift(Ball_lift::IN),
	shooter_belt(Shooter_belt::DISABLED),
	ball_collector(Ball_collector::DISABLED),
	auto_select(0),
	speed_dial(0)
{}

ostream& operator<<(ostream& o,Panel::Ball_arm a){
	#define X(NAME) if(a==Panel::Ball_arm::NAME) return o<<""#NAME;
	X(UP) X(DOWN) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Gear_grasper a){
	#define X(NAME) if(a==Panel::Gear_grasper::NAME) return o<<""#NAME;
	X(OPEN) X(CLOSED) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Gear_collector a){
	#define X(NAME) if(a==Panel::Gear_collector::NAME) return o<<""#NAME;
	X(UP) X(DOWN) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Shooter a){
	#define X(NAME) if(a==Panel::Shooter::NAME) return o<<""#NAME;
	X(ENABLED) X(DISABLED) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Ball_intake a){
	#define X(NAME) if(a==Panel::Ball_intake::NAME) return o<<""#NAME;
	X(OUT) X(IN) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Ball_lift a){
	#define X(NAME) if(a==Panel::Ball_lift::NAME) return o<<""#NAME;
	X(OUT) X(IN) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Shooter_belt a){
	#define X(NAME) if(a==Panel::Shooter_belt::NAME) return o<<""#NAME;
	X(ENABLED) X(DISABLED) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Ball_collector a){
	#define X(NAME) if(a==Panel::Ball_collector::NAME) return o<<""#NAME;
	X(DISABLED) X(AUTO)
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
		float auto_dial_value = d.axis[AUTO_SELECTOR_AXIS];
		p.auto_select = interpret_20_turn_pot(auto_dial_value);
	}
	{//two position switches
	}
	{//three position switches
		float ball_arm = d.axis[BALL_ARM_AXIS];
		p.ball_arm = [&]{
			static const float AUTO=-1,DOWN=0,UP=1;
			if(set_button(ball_arm,AUTO,DOWN,UP)) return Panel::Ball_arm::DOWN;
			if(set_button(ball_arm,DOWN,UP,ARTIFICIAL_MAX)) return Panel::Ball_arm::UP;
			return Panel::Ball_arm::AUTO;
		}();
		
		float gear_grasper = d.axis[GEAR_GRASPER_AXIS];
		p.gear_grasper = [&]{
			static const float AUTO=-1,CLOSED=0,OPEN=1;
			if(set_button(gear_grasper,AUTO,CLOSED,OPEN)) return Panel::Gear_grasper::CLOSED;
			if(set_button(gear_grasper,CLOSED,OPEN,ARTIFICIAL_MAX)) return Panel::Gear_grasper::OPEN;
			return Panel::Gear_grasper::AUTO;
		}();

		float gear_collector = d.axis[GEAR_COLLECTOR_AXIS];
		p.gear_collector = [&]{
			static const float AUTO=-1,DOWN=0,UP=1;
			if(set_button(gear_collector,AUTO,DOWN,UP)) return Panel::Gear_collector::DOWN;
			if(set_button(gear_collector,DOWN,UP,ARTIFICIAL_MAX)) return Panel::Gear_collector::UP;
			return Panel::Gear_collector::AUTO;
		}();

		float shooter = d.axis[SHOOTER_AXIS];
		p.shooter = [&]{
			static const float AUTO=-1,DISABLED=0,ENABLED=1;
			if(set_button(shooter,AUTO,DISABLED,ENABLED)) return Panel::Shooter::DISABLED;
			if(set_button(shooter,DISABLED,ENABLED,ARTIFICIAL_MAX)) return Panel::Shooter::ENABLED;
			return Panel::Shooter::AUTO;
		}();

		float ball_intake = d.axis[BALL_INTAKE_AXIS];
		p.ball_intake = [&]{
			static const float AUTO=-1,IN=0,OUT=1;
			if(set_button(ball_intake,AUTO,IN,OUT)) return Panel::Ball_intake::IN;
			if(set_button(ball_intake,IN,OUT,ARTIFICIAL_MAX)) return Panel::Ball_intake::OUT;
			return Panel::Ball_intake::AUTO;
		}();

		float ball_lift = d.axis[BALL_LIFT_AXIS];
		p.ball_lift = [&]{
			static const float AUTO=-1,IN=0,OUT=1;
			if(set_button(ball_lift,AUTO,IN,OUT)) return Panel::Ball_lift::IN;
			if(set_button(ball_lift,IN,OUT,ARTIFICIAL_MAX)) return Panel::Ball_lift::OUT;
			return Panel::Ball_lift::AUTO;
		}();

		bool shooter_belt_enabled = d.button[SHOOTER_BELT_ENABLED_LOC], shooter_belt_auto = d.button[SHOOTER_BELT_AUTO_LOC];
		p.shooter_belt = [&]{
			if(shooter_belt_enabled) return Panel::Shooter_belt::ENABLED;
			if(shooter_belt_auto) return Panel::Shooter_belt::AUTO;
			return Panel::Shooter_belt::DISABLED;
		}();

		bool /*ball_collector_disabled = d.button[BALL_COLLECTOR_DISABLED_LOC],*/ ball_collector_auto = d.button[BALL_COLLECTOR_AUTO_LOC];
		p.ball_collector = [&]{
			if(ball_collector_auto) return Panel::Ball_collector::AUTO;
			return Panel::Ball_collector::DISABLED;
		}();
	}
	{//buttons
		p.ball_collect=d.button[BALL_COLLECT_LOC];
		p.loading_indicator=d.button[LOADING_INDICATOR_LOC];
		p.shoot=d.button[SHOOT_LOC];
		p.gear_prep_collect=d.button[PREP_COLLECT_GEAR_LOC];
		p.gear_prep_score=d.button[PREP_SCORE_GEAR_LOC];
		p.gear_collect=d.button[COLLECT_GEAR_LOC];
		p.gear_score=d.button[SCORE_GEAR_LOC];
		p.climb=d.button[CLIMB_LOC];
		p.learn=d.button[LEARN_LOC];
	}
	{//Dials
		p.speed_dial = -d.axis[SPEED_DIAL_AXIS];
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
	
	//TODO: Redo this with new controls
	/*
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
	*/
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
