#ifndef AUTO_GUIDED_TURN_H
#define AUTO_GUIDED_TURN_H

#include "executive.h"
#include "../util/motion_profile.h"
#include "../util/quick.h"

struct Auto_guided_turn: public Executive_impl<Auto_guided_turn>{
	#define AUTO_GUIDED_TURN_ITEMS(X)\
		X(Countdown_timer,in_range) 

	STRUCT_MEMBERS(AUTO_GUIDED_TURN_ITEMS)
	
	IMPL_STRUCT(Auto_guided_turn,AUTO_GUIDED_TURN_ITEMS)
	Auto_guided_turn(){}

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_guided_turn const&)const;
};
#endif

