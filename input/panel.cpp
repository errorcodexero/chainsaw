#include "panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>

using namespace std;
static const unsigned int BALL_ARM_AXIS=2, BALL_INTAKE_AXIS=1, BALL_COLLECTOR_AXIS=0, SHOOTER_AXIS=3, GEAR_ARM_AXIS=5, GEAR_GRABBER_AXIS=4, AUTO_SELECTOR_AXIS=6, SPEED_DIAL_AXIS=7;//TODO: rename constants
static const unsigned int BALL_COLLECT_LOC=0, LOADING_INDICATOR_LOC=1, SHOOT_LOC=2, PREP_COLLECT_GEAR_LOC=3, PREP_SCORE_GEAR_LOC=4, COLLECT_GEAR_LOC=5, SCORE_GEAR_LOC=6, CLIMB_LOC=7, LEARN_LOC=8, SHOOTER_BELT_AUTO_LOC=11, SHOOTER_BELT_ENABLED_LOC=12, BALL_LIFTER_IN_LOC=9, BALL_LIFTER_OUT_LOC=10;//TODO: rename constants 

#define BUTTONS \
	X(ball_collect) X(camera_light) X(shoot) X(gear_prep_collect) X(gear_prep_score) X(gear_collect) X(gear_score) X(climb) X(learn)
#define THREE_POS_SWITCHES \
	X(ball_arm) X(gear_grabber) X(gear_arm) X(shooter) X(ball_intake) X(ball_lift) X(shooter_belt) X(ball_collector)
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
	ball_arm(Ball_arm::STOW),
	gear_grabber(Gear_grabber::CLOSED),
	gear_arm(Gear_arm::DOWN),
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
	X(STOW) X(LOW) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Gear_grabber a){
	#define X(NAME) if(a==Panel::Gear_grabber::NAME) return o<<""#NAME;
	X(OPEN) X(CLOSED) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Gear_arm a){
	#define X(NAME) if(a==Panel::Gear_arm::NAME) return o<<""#NAME;
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
			static const float AUTO=-1,LOW=0,STOW=1;
			if(set_button(ball_arm,AUTO,LOW,STOW)) return Panel::Ball_arm::LOW;
			if(set_button(ball_arm,LOW,STOW,ARTIFICIAL_MAX)) return Panel::Ball_arm::STOW;
			return Panel::Ball_arm::AUTO;
		}();
		
		float gear_grabber = d.axis[GEAR_GRABBER_AXIS];
		p.gear_grabber = [&]{
			static const float AUTO=-1,CLOSED=0,OPEN=1;
			if(set_button(gear_grabber,AUTO,CLOSED,OPEN)) return Panel::Gear_grabber::CLOSED;
			if(set_button(gear_grabber,CLOSED,OPEN,ARTIFICIAL_MAX)) return Panel::Gear_grabber::OPEN;
			return Panel::Gear_grabber::AUTO;
		}();

		float gear_arm = d.axis[GEAR_ARM_AXIS];
		p.gear_arm = [&]{
			static const float AUTO=-1,DOWN=0,UP=1;
			if(set_button(gear_arm,AUTO,DOWN,UP)) return Panel::Gear_arm::DOWN;
			if(set_button(gear_arm,DOWN,UP,ARTIFICIAL_MAX)) return Panel::Gear_arm::UP;
			return Panel::Gear_arm::AUTO;
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
			static const float AUTO=-1,OUT=0,IN=1;
			if(set_button(ball_intake,AUTO,OUT,IN)) return Panel::Ball_intake::OUT;
			if(set_button(ball_intake,OUT,IN,ARTIFICIAL_MAX)) return Panel::Ball_intake::IN;
			return Panel::Ball_intake::AUTO;
		}();

		bool shooter_belt_enabled = d.button[SHOOTER_BELT_ENABLED_LOC], shooter_belt_auto = d.button[SHOOTER_BELT_AUTO_LOC];
		p.shooter_belt = [&]{
			if(shooter_belt_enabled) return Panel::Shooter_belt::ENABLED;
			if(shooter_belt_auto) return Panel::Shooter_belt::AUTO;
			return Panel::Shooter_belt::DISABLED;
		}();

		float ball_collector = d.axis[BALL_COLLECTOR_AXIS];
		p.ball_collector = [&]{
			static const float AUTO=-1,DISABLED=1;
			if(set_button(ball_collector,AUTO,DISABLED,DISABLED)) return Panel::Ball_collector::DISABLED;
			if(set_button(ball_collector,DISABLED,DISABLED,ARTIFICIAL_MAX)) return Panel::Ball_collector::DISABLED;
			return Panel::Ball_collector::AUTO;
		}();
		
		bool lift_in  = d.button[BALL_LIFTER_IN_LOC], lift_auto = d.button[BALL_LIFTER_OUT_LOC];
		p.ball_lift = [&]{
			if(lift_in) return Panel::Ball_lift::AUTO;
			if(lift_auto) return Panel::Ball_lift::OUT;
			return Panel::Ball_lift::IN;
		}();
	
	}	
	{//buttons
		p.ball_collect=d.button[BALL_COLLECT_LOC];
		p.camera_light=d.button[LOADING_INDICATOR_LOC];
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
	
	bool alternative_op = d.button[Gamepad_button::LB];
	p.auto_select=0;
	p.speed_dial = (d.axis[Gamepad_axis::LTRIGGER]-.5)*2;
	p.learn = d.button[Gamepad_button::START];
	p.camera_light = d.button[Gamepad_button::BACK];

	if(!alternative_op){
		p.gear_prep_score = d.button[Gamepad_button::Y];
		p.gear_score = d.button[Gamepad_button::B];
		p.gear_collect = d.button[Gamepad_button::X];
		p.gear_prep_collect = d.button[Gamepad_button::A];

		cout<<"\nPOV:"<<d.pov<<"\n";

		switch(pov_section(d.pov)){
			case POV_section::CENTER:
				break;
			case POV_section::UP:
				p.shoot = true;
				break;
			case POV_section::UP_LEFT:
				break;
			case POV_section::LEFT:
				p.climb = true;
				break;
			case POV_section::DOWN_LEFT:
				break;
			case POV_section::DOWN:
				break;
			case POV_section::DOWN_RIGHT:
				break;
			case POV_section::RIGHT:
				p.ball_collect=true;
				break;
			case POV_section::UP_RIGHT:
				break;
			default:
				assert(0);
		}

		p.ball_arm=Panel::Ball_arm::AUTO;
		p.gear_grabber=Panel::Gear_grabber::AUTO;
		p.gear_arm=Panel::Gear_arm::AUTO;
		p.shooter=Panel::Shooter::AUTO;
		p.ball_intake=Panel::Ball_intake::AUTO;
		p.ball_lift=Panel::Ball_lift::AUTO;
		p.shooter_belt=Panel::Shooter_belt::AUTO;
		p.ball_collector=Panel::Ball_collector::AUTO;

		p.ball_arm=Panel::Ball_arm::STOW;
	} else {
		p.ball_collector = d.button[Gamepad_button::RB] ? Panel::Ball_collector::DISABLED:Panel::Ball_collector::AUTO;

		if(d.button[Gamepad_button::B]) p.gear_grabber=Panel::Gear_grabber::CLOSED;
		else if(!d.button[Gamepad_button::X]) p.gear_grabber= Panel::Gear_grabber::OPEN;
		else p.gear_grabber=Panel::Gear_grabber::AUTO;

		if(d.button[Gamepad_button::Y]) p.gear_arm=Panel::Gear_arm::UP;
		else if(!d.button[Gamepad_button::A]) p.gear_arm=Panel::Gear_arm::DOWN;
		else p.gear_arm=Panel::Gear_arm::AUTO;

		p.ball_lift=Panel::Ball_lift::AUTO;
		p.ball_intake=Panel::Ball_intake::AUTO;
		switch(pov_section(d.pov)){
			case POV_section::CENTER:
				break;
			case POV_section::UP:
				p.ball_lift=Panel::Ball_lift::IN;
				break;
			case POV_section::UP_LEFT:
				break;
			case POV_section::LEFT:
				p.ball_intake=Panel::Ball_intake::IN;
				break;
			case POV_section::DOWN_LEFT:
				break;
			case POV_section::DOWN:
				p.ball_lift=Panel::Ball_lift::OUT;
				break;
			case POV_section::DOWN_RIGHT:
				break;
			case POV_section::RIGHT:
				p.ball_intake=Panel::Ball_intake::OUT;
				break;
			case POV_section::UP_RIGHT:
				break;
			default:
				assert(0);
		}

		switch(joystick_section(d.axis[Gamepad_axis::LEFTX],d.axis[Gamepad_axis::LEFTY])){
			case Joystick_section::UP:
				p.shooter=Panel::Shooter::ENABLED;
				p.shooter_belt=Panel::Shooter_belt::DISABLED;
				break;
			case Joystick_section::LEFT:
				p.shooter=Panel::Shooter::DISABLED;
				p.shooter_belt=Panel::Shooter_belt::DISABLED;
				break;
			case Joystick_section::DOWN:
				p.shooter=Panel::Shooter::DISABLED;
				p.shooter_belt=Panel::Shooter_belt::ENABLED;
				break;
			case Joystick_section::RIGHT:
				p.shooter=Panel::Shooter::ENABLED;
				p.shooter_belt=Panel::Shooter_belt::ENABLED;
				break;
			case Joystick_section::CENTER:
				p.shooter=Panel::Shooter::AUTO;
				p.shooter_belt=Panel::Shooter_belt::AUTO;
				break;
			default:
				assert(0);
		}

		switch(joystick_section(d.axis[Gamepad_axis::RIGHTX],d.axis[Gamepad_axis::RIGHTY])){
			case Joystick_section::UP:
				p.ball_arm=Panel::Ball_arm::STOW;
				break;
			case Joystick_section::LEFT:
				break;
			case Joystick_section::DOWN:
				p.ball_arm=Panel::Ball_arm::LOW;
				break;
			case Joystick_section::RIGHT:
				break;
			case Joystick_section::CENTER:
				p.ball_arm=Panel::Ball_arm::AUTO;
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
