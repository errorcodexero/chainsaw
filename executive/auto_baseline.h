#ifndef BASELINE_H
#define BASELINE_H

#include "executive.h"
#include "../util/motion_profile.h"
#include "../util/quick.h"


struct Auto_baseline: public Executive_impl<Auto_baseline>{

	#define AUTO_BASELINE_ITEMS(X)\
	X(Motion_profile,motion_profile)\
	X(Drivebase::Encoder_ticks,initial_encoders) \
	X(Countdown_timer,in_range) 

	STRUCT_MEMBERS(AUTO_BASELINE_ITEMS)
	
	Auto_baseline(Drivebase::Encoder_ticks initial_encoders1):initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_baseline,AUTO_BASELINE_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_baseline const&)const;
};
#endif

