#ifndef AUTO_GEARBOILEREXT_H
#define AUTO_GEARBOILEREXT_H

#include "executive.h"
#include "../util/motion_profile.h"
#include "../util/quick.h"

#define AUTO_GEARBOILEREXT_ITEMS(X)\
	X(unsigned,gear_step)\
	X(Motion_profile,motion_profile)\
	X(SINGLE_ARG(std::pair<int,int>),initial_encoders)\

struct Auto_gearboilerext_frompeg : public Executive_impl<Auto_gearboilerext_frompeg>{
	#define AUTO_GEARBOILEREXT_FROMPEG_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARBOILEREXT_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARBOILEREXT_FROMPEG_ITEMS)
	Auto_gearboilerext_frompeg(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}	
	IMPL_STRUCT(Auto_gearboilerext_frompeg,AUTO_GEARBOILEREXT_FROMPEG_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearboilerext_frompeg const&)const;
};

struct Auto_gearboilerext_turn : public Executive_impl<Auto_gearboilerext_turn>{
	 #define AUTO_GEARBOILEREXT_TURN_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARBOILEREXT_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARBOILEREXT_TURN_ITEMS)
	Auto_gearboilerext_turn(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearboilerext_turn,AUTO_GEARBOILEREXT_TURN_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearboilerext_turn const&)const;
};

struct Auto_gearboilerext_driveone: public Executive_impl<Auto_gearboilerext_driveone>{
	#define AUTO_GEARBOILEREXT_DRIVEONE_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARBOILEREXT_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARBOILEREXT_DRIVEONE_ITEMS)
	
	Auto_gearboilerext_driveone(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearboilerext_driveone,AUTO_GEARBOILEREXT_DRIVEONE_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearboilerext_driveone const&)const;
};

struct Auto_gearboilerext_turntwo: public Executive_impl<Auto_gearboilerext_turntwo>{
	#define AUTO_GEARBOILEREXT_TURNTWO_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARBOILEREXT_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARBOILEREXT_TURNTWO_ITEMS)

	Auto_gearboilerext_turntwo(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearboilerext_turntwo,AUTO_GEARBOILEREXT_TURNTWO_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearboilerext_turntwo const&)const;
};

struct Auto_gearboilerext_drivetwo: public Executive_impl<Auto_gearboilerext_drivetwo>{
	#define AUTO_GEARBOILEREXT_DRIVETWO_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARBOILEREXT_ITEMS(X)	
	STRUCT_MEMBERS(AUTO_GEARBOILEREXT_DRIVETWO_ITEMS)

	Auto_gearboilerext_drivetwo(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){} 
	IMPL_STRUCT(Auto_gearboilerext_drivetwo,AUTO_GEARBOILEREXT_DRIVETWO_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearboilerext_drivetwo const&)const;
};


#endif
