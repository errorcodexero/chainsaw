#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"

struct Panel{
	bool in_use;
	//*Buttons:
	bool learn;
	bool collect;
	bool shoot_prep;
	bool shoot;
	bool gear_collect;
	bool gear_prep_collect;
	bool gear_prep_score;
	bool gear_score;
	bool climb;
	//2 position swicthes:
	//3 position switches:
	enum class Gear_lifter{UP,DOWN,AUTO};
	Gear_lifter gear_lifter;
	enum class Gear_grabber{OPEN,CLOSE,AUTO};
	Gear_grabber gear_grabber;
	enum class Shooter{ON,OFF,REVERSE};
	Shooter shooter;
	enum class Arm{STOW,LOW,AUTO};
	Arm arm;
	enum class Intake_direction{IN,OUT,AUTO};
	Intake_direction intake_direction;
	enum class Intake_control{ON,OFF,AUTO};//for intake//TODO
	Intake_control intake_control; 
	enum class Lift{UP,DOWN,AUTO};
	Lift lift;
	//10 *position switches:
	int auto_select;//0-19
	//Dials:
	float speed_dial;
	Panel();
};

bool operator!=(Panel const&,Panel const&);
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret_oi(Joystick_data);
Panel interpret_test_oi(Joystick_data);
Panel interpret_gamepad(Joystick_data);

Joystick_data driver_station_input_rand();
Panel rand(Panel*);

#endif
