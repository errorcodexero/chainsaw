
#ifndef AUTO_GEARMID_H
#define AUTO_GEARMID_H

#include "executive.h"
#include "../util/motion_profile.h"
#include "../util/quick.h"

#define AUTO_GEARMID_ITEMS(X)\
	X(unsigned,gear_step)\
	X(Motion_profile,motion_profile)\
	X(SINGLE_ARG(std::pair<int,int>),initial_encoders)\
	X(bool,encoderflag)

struct Auto_gearmid_topeg : public Executive_impl<Auto_gearmid_topeg>{
	#define AUTO_GEARMID_TOPEG_ITEMS(X)\
		X(Countdown_timer,in_auto_range)\
		AUTO_GEARMID_ITEMS(X)
	STRUCT_MEMBERS(AUTO_GEARMID_TOPEG_ITEMS)
	
	Auto_gearmid_topeg(unsigned gear_step1,std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1),encoderflag(0){}
	Auto_gearmid_topeg():gear_step(0),initial_encoders({0,0}),encoderflag(0){}
	IMPL_STRUCT(Auto_gearmid_topeg,AUTO_GEARMID_TOPEG_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearmid_topeg const&)const;
};

struct Auto_gearmid_geardrop : public Executive_impl<Auto_gearmid_geardrop>{
	STRUCT_MEMBERS(AUTO_GEARMID_ITEMS)
	
	Auto_gearmid_geardrop(unsigned gear_step1, std::pair<int,int> initial_encoders1):gear_step(gear_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_gearmid_geardrop,AUTO_GEARMID_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_gearmid_geardrop const&)const;
};


#endif

