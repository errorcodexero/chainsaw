#ifndef TEST_PANEL_H
#define TEST_PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"
#include "panel.h"

struct Test_panel{
	bool in_use;
	//Buttons:
	bool button_0;
	bool button_1;
	bool button_2;
	bool button_3;
	//2 position swicthes:
	bool two_pos_switch_0;
	bool two_pos_switch_1;
	//3 position switches: 
	//10 position switches:
	int ten_pos_switch;//0-9//TODO: this is acutally a twenty position switch: a ten position switch connected to a three position switch: varies axis values not separate input
	//Dials:
	double potentiometer;
		
	Test_panel();
};

Test_panel interpret_test_oi(Joystick_data);
bool operator!=(Test_panel const&,Test_panel const&);
std::ostream& operator<<(std::ostream&,Test_panel);

Test_panel rand(Test_panel*);

#endif
