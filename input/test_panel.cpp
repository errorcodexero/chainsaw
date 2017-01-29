#include "test_panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>

using namespace std;

static const unsigned int POTENTIOMETER_AXIS=0; //TODO: need real values
static const unsigned int BUTTON_0_LOC=0, BUTTON_1_LOC=1, BUTTON_2_LOC=2, BUTTON_3_LOC=3; //TODO: need real values

#define BUTTONS \
	X(button_0) \
	X(button_1) \
	X(button_2) \
	X(button_3) 

#define TWO_POS_SWITCHES \

#define THREE_POS_SWITCHES \

#define TEN_POS_SWITCHES \
	X(ten_pos_switch)

#define DIALS \
	X(potentiometer)

#define PANEL_ITEMS \
	BUTTONS \
	TWO_POS_SWITCHES \
	THREE_POS_SWITCHES \
	TEN_POS_SWITCHES \
	DIALS


Test_panel::Test_panel():
	in_use(0),
	#define X(BUTTON) BUTTON(false),
	BUTTONS
	#undef X
	#define X(TWO_POS_SWITCH) TWO_POS_SWITCH(false),
	TWO_POS_SWITCHES
	#undef X
	ten_pos_switch(0)
{}

ostream& operator<<(ostream& o,Test_panel p){
	o<<"Test_panel(";
	o<<"in_use:"<<p.in_use;
	#define X(NAME) o<<", "#NAME":"<<p.NAME;
	PANEL_ITEMS
	#undef X
	return o<<")";
}

bool operator==(Test_panel const& a,Test_panel const& b){
	return true
	#define X(NAME) && a.NAME==b.NAME
	PANEL_ITEMS
	#undef X
	;
}

bool operator!=(Test_panel const& a,Test_panel const& b){
	return !(a==b);
}

Test_panel interpret_test_oi(Joystick_data d){
	Test_panel p;
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
	{//set the auto mode number from the dial value
		Volt auto_dial_value = d.axis[0];
		p.ten_pos_switch = interpret_10_turn_pot(auto_dial_value);
	}
	{//two position switches
	}
	{//buttons
		p.button_0 = d.button[BUTTON_0_LOC];
		p.button_1 = d.button[BUTTON_1_LOC];
		p.button_2 = d.button[BUTTON_2_LOC];
		p.button_3 = d.button[BUTTON_3_LOC];
	}
	{//dials
		p.potentiometer = d.axis[POTENTIOMETER_AXIS];
	}
	return p;
}

Test_panel rand(Test_panel*){
	return interpret_test_oi(driver_station_input_rand());
}

#ifdef PANEL_TEST
int main(){
	Test_panel p;
	for(unsigned i=0;i<50;i++){
		interpret_test_oi(driver_station_input_rand());
	}
	cout<<p<<"\n";
	return 0;
}
#endif
