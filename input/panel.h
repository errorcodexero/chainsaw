#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"

struct Panel{
	bool in_use;
	//*Buttons:
	bool ball_collect;
	bool loading_indicator;
	bool shoot;
	bool gear_prep_collect;
	bool gear_prep_score;
	bool gear_collect;
	bool gear_score;
	bool climb;
	bool learn;
	//2 position swicthes:
	//3 position switches:
	enum class Ball_arm{UP,DOWN,AUTO};
	Ball_arm ball_arm;
	enum class Gear_grasper{OPEN,CLOSED,AUTO};
	Gear_grasper gear_grasper;
	enum class Gear_collector{UP,DOWN,AUTO};
	Gear_collector gear_collector;
	enum class Shooter{ENABLED,DISABLED,AUTO};
	Shooter shooter;
	enum class Ball_intake{OUT,IN,AUTO};
	Ball_intake ball_intake;
	enum class Ball_lift{OUT,IN,AUTO};
	Ball_lift ball_lift;
	enum class Shooter_belt{ENABLED,DISABLED,AUTO};
	Shooter_belt shooter_belt;
	enum class Ball_collector{DISABLED,AUTO};
	Ball_collector ball_collector;
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
