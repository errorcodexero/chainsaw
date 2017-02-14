#ifndef AUTO_GEARBOILER_H
#define AUTO_GEARBOILER_H

#include "executive.h"
#include "../util/motion_profile.h"
#include "../util/quick.h"

#define AUTO_GEARBOILER_ITEMS(X)\
	X(unsigned,gear_step)\
	X(Motion_profile,motion_profile)\
	X(SINGLE_ARG(std::pair<int,int>),initial_encoders)

struct Auto_gearboiler_topeg : public Executive_impl<Auto_gearboiler_topeg>{
	#define AUTO_GEARBOILER_TOPEG_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARBOILER_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARBOILER_TOPEG_ITEMS)
	
	Auto_gearboiler_topeg(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearboiler_topeg,AUTO_GEARBOILER_TOPEG_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearboiler_topeg const&)const;
};

struct Auto_gearboiler_turn : public Executive_impl<Auto_gearboiler_turn>{
	 #define AUTO_GEARBOILER_TURN_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARBOILER_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARBOILER_TURN_ITEMS)

	Auto_gearboiler_turn(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearboiler_turn,AUTO_GEARBOILER_TURN_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearboiler_turn const&)const;
};

struct Auto_gearboiler_approach: public Executive_impl<Auto_gearboiler_approach>{
	#define AUTO_GEARBOILER_APPROACH_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARBOILER_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARBOILER_APPROACH_ITEMS)

	Auto_gearboiler_approach(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearboiler_approach,AUTO_GEARBOILER_APPROACH_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearboiler_approach const&)const;
};


struct Auto_gearboiler_geardrop : public Executive_impl<Auto_gearboiler_geardrop>{
	STRUCT_MEMBERS(AUTO_GEARBOILER_ITEMS)
	
	Auto_gearboiler_geardrop(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearboiler_geardrop,AUTO_GEARBOILER_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearboiler_geardrop const&)const;
};


#endif
