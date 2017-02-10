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
	enum class Gear_collector{UP,DOWN,AUTO};//gear_lifter
	Gear_collector gear_collector;
	enum class Gear_holder{OPEN,CLOSED,AUTO};//gear_grabber
	Gear_holder gear_holder;
	enum class Ball_shooter{POWER,RPM,AUTO};
	Ball_shooter ball_shooter;
	enum class Ball_arm{STOW,LOW,AUTO};//arm
	Ball_arm ball_arm;
	enum class Belt_direction{IN,OUT,AUTO};//lift
	Belt_direction belt_direction;
	enum class Ball_collector{ON,OFF,AUTO};//intake
	Ball_collector ball_collector; 
	//10 *position switches:
	int auto_select;//0-9
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
