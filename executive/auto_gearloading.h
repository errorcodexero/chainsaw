#ifndef AUTO_GEARLOADING_H
#define AUTO_GEARLOADING_H

#include "executive.h"
#include "../util/motion_profile.h"
#include "../util/quick.h"

#define AUTO_GEARLOADING_ITEMS(X)\
	X(unsigned,gear_step)\
	X(Motion_profile,motion_profile)\
	X(SINGLE_ARG(std::pair<int,int>),initial_encoders)

struct Auto_gearloading_topeg : public Executive_impl<Auto_gearloading_topeg>{
	#define AUTO_GEARLOADING_TOPEG_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARLOADING_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARLOADING_TOPEG_ITEMS)
	
	Auto_gearloading_topeg(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearloading_topeg,AUTO_GEARLOADING_TOPEG_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearloading_topeg const&)const;
};

struct Auto_gearloading_turn : public Executive_impl<Auto_gearloading_turn>{
	 #define AUTO_GEARLOADING_TURN_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARLOADING_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARLOADING_TURN_ITEMS)

	Auto_gearloading_turn(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearloading_turn,AUTO_GEARLOADING_TURN_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearloading_turn const&)const;
};

struct Auto_gearloading_approach: public Executive_impl<Auto_gearloading_approach>{
	#define AUTO_GEARLOADING_APPROACH_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARLOADING_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARLOADING_APPROACH_ITEMS)

	Auto_gearloading_approach(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearloading_approach,AUTO_GEARLOADING_APPROACH_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearloading_approach const&)const;
};


struct Auto_gearloading_geardrop : public Executive_impl<Auto_gearloading_geardrop>{
	STRUCT_MEMBERS(AUTO_GEARLOADING_ITEMS)
	
	Auto_gearloading_geardrop(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearloading_geardrop,AUTO_GEARLOADING_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearloading_geardrop const&)const;
};


#endif
