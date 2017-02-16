#ifndef BASELINEEXT_H
#define BASELINEEXT_H

#include "executive.h"
#include "../util/motion_profile.h"
#include "../util/quick.h"


struct Auto_baselineext: public Executive_impl<Auto_baselineext>{

	#define AUTO_BASELINEEXT_ITEMS(X)\
	X(Motion_profile,motion_profile)\
	X(Drivebase::Encoder_ticks,initial_encoders) \
	X(Countdown_timer,in_range) 

	STRUCT_MEMBERS(AUTO_BASELINEEXT_ITEMS)
	
	Auto_baselineext(Drivebase::Encoder_ticks initial_encoders1):initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_baselineext,AUTO_BASELINEEXT_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_baselineext const&)const;
};
#endif

