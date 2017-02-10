#include "panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>

using namespace std;
static const unsigned int BALL_COL_AXIS=0,BELT_DIR_AXIS=1,BALL_ARM_AXIS=2,BALL_SHOOTER_AXIS=3,GEAR_HOLDER_AXIS=4,GEAR_COL_AXIS=5,AUTO_SEL_AXIS=6,SHOOTER_DIAL_AXIS=7;
static const unsigned int BALL_COL_LOC=0,PREP_SHOOT_LOC=1,SHOOT_LOC=2,COL_GEAR_LOC=3,PREP_GEAR_LOC=4,PREP_SCORE_GEAR_LOC=5,SCORE_GEAR_LOC=6,CLIMBER_LOC=8,LEARN_LOC=9;

#define BUTTONS \
	X(learn) X(collect) X(shoot_prep) X(shoot) X(gear_collect) X(gear_prep_collect) X(gear_prep_score) X(gear_score) X(climb)
#define THREE_POS_SWITCHES \
	X(gear_collector) X(gear_holder) X(ball_shooter) X(ball_arm) X(belt_direction) X(ball_collector)
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
	gear_collector(Gear_collector::UP),
	gear_holder(Gear_holder::OPEN),
	ball_shooter(Ball_shooter::POWER),
	ball_arm(Ball_arm::STOW),
	belt_direction(Belt_direction::IN),
	ball_collector(Ball_collector::OFF),
	auto_select(0)
{}

ostream& operator<<(ostream& o,Panel::Gear_collector p){
	#define X(NAME) if(p==Panel::Gear_collector::NAME) return o<<""#NAME;
	X(UP) X(DOWN) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Gear_holder p){
	#define X(NAME) if(p==Panel::Gear_holder::NAME) return o<<""#NAME;
	X(OPEN) X(CLOSED) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Ball_shooter p){
	#define X(NAME) if(p==Panel::Ball_shooter::NAME) return o<<""#NAME;
	X(POWER) X(RPM) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Ball_arm p){
	#define X(NAME) if(p==Panel::Ball_arm::NAME) return o<<""#NAME;
	X(STOW) X(STOW) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Belt_direction p){
	#define X(NAME) if(p==Panel::Belt_direction::NAME) return o<<""#NAME;
	X(IN) X(OUT) X(AUTO)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Ball_collector p){
	#define X(NAME) if(p==Panel::Ball_collector::NAME) return o<<""#NAME;
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

bool set_button(const Volt AXIS_VALUE, const Volt LOWER_VALUE, const Volt TESTING_VALUE, const Volt UPPER_VALUE){
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
	static const Volt ARTIFICIAL_MAX = 1.5;
	{
		p.in_use=get_in_use(d);
		if(!p.in_use) return p;
	}
	{//set the auto mode number from the dial value
		Volt auto_dial_value = d.axis[AUTO_SEL_AXIS];
		p.auto_select = interpret_10_turn_pot(auto_dial_value);
	}
	{//two position switches
	}
	{//three position switches
		
		Volt gear_collector = d.axis[GEAR_COL_AXIS];
		p.gear_collector = [&]{
			static const Volt UP = -1, DOWN = 0, AUTO = 1;
			if(set_button(gear_collector,UP,DOWN,AUTO)) return Panel::Gear_collector::DOWN;
			if(set_button(gear_collector,DOWN,AUTO,ARTIFICIAL_MAX)) return Panel::Gear_collector::AUTO;
			return Panel::Gear_collector::UP;
		}();

		Volt gear_holder = d.axis[GEAR_HOLDER_AXIS];
			p.gear_holder = [&]{
			static const Volt OPEN = -1, CLOSED = 0, AUTO = 1;
			if(set_button(gear_holder,OPEN,CLOSED,AUTO)) return Panel::Gear_holder::CLOSED;
			if(set_button(gear_holder,CLOSED,AUTO,ARTIFICIAL_MAX)) return Panel::Gear_holder::AUTO;
			return Panel::Gear_holder::OPEN;
		}();	
	 
		Volt ball_shooter = d.axis[BALL_SHOOTER_AXIS];
			p.ball_shooter = [&]{
			static const Volt POWER = -1, RPM = 0, AUTO = 1;
			if(set_button(ball_shooter,POWER,RPM,AUTO)) return Panel::Ball_shooter::RPM;
			if(set_button(ball_shooter,RPM,AUTO,ARTIFICIAL_MAX)) return Panel::Ball_shooter::AUTO;
			return Panel::Ball_shooter::POWER;
		}();
		
		
		Volt ball_arm = d.axis[BALL_ARM_AXIS];
			p.ball_arm = [&]{
			static const Volt STOW = -1, LOW = 0, AUTO = 1;
			if(set_button(ball_arm,STOW,LOW,AUTO)) return Panel::Ball_arm::LOW;
			if(set_button(ball_arm,LOW,AUTO,ARTIFICIAL_MAX)) return Panel::Ball_arm::AUTO;
			return Panel::Ball_arm::STOW;
		}();
			
		Volt belt_direction = d.axis[BELT_DIR_AXIS];
			p.belt_direction = [&]{
			static const Volt IN = -1, OUT = 0, AUTO = 1;
			if(set_button(belt_direction,IN,OUT,AUTO)) return Panel::Belt_direction::OUT;
			if(set_button(belt_direction,IN,AUTO,ARTIFICIAL_MAX)) return Panel::Belt_direction::AUTO;
			return Panel::Belt_direction::IN;
		}();

		Volt ball_collector = d.axis[BALL_COL_AXIS];
			p.ball_collector = [&]{
			static const Volt ON = -1, OFF = 0, AUTO = 1;
			if(set_button(ball_collector,ON,OFF,AUTO)) return Panel::Ball_collector::OFF;
			if(set_button(ball_collector,OFF,AUTO,ARTIFICIAL_MAX)) return Panel::Ball_collector::AUTO;
			return Panel::Ball_collector::ON;
		}();

	
	}
	{//buttons
		//sets all buttons to off beacuse we assume that only one should be pressed on this axis at a time
		#define X(button) p.button = false;
		BUTTONS
		#undef X
		p.learn=d.button[LEARN_LOC];
		p.collect=d.button[BALL_COL_LOC];
		p.shoot_prep=d.button[PREP_SHOOT_LOC];
		p.shoot=d.button[SHOOT_LOC];
		p.gear_collect=d.button[COL_GEAR_LOC];
		p.gear_prep_collect=d.button[PREP_GEAR_LOC];
		p.gear_prep_score=d.button[PREP_SCORE_GEAR_LOC];
		p.gear_score=d.button[SCORE_GEAR_LOC];
		p.climb=d.button[CLIMBER_LOC];
		
		//const Volt AXIS_VALUE = d.axis[2];
		//static const Volt DEFAULT=-1, ARTIFICIAL_MAX = 1.38;
	
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
		
	} else{
		
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
