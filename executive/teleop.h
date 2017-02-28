#ifndef TELEOP_H
#define TELEOP_H

#include "executive.h"
#include "../util/posedge_trigger_debounce.h"
#include "../util/posedge_toggle.h"
#include "../util/quick.h"

struct Teleop : Executive_impl<Teleop> {
	enum Nudges{FORWARD,BACKWARD,CLOCKWISE,COUNTERCLOCKWISE,NUDGES};
	#define NUDGE_ITEMS(X) X(Posedge_trigger,trigger) X(Countdown_timer,timer)
	struct Nudge{
		Posedge_trigger trigger;
		Countdown_timer timer;
	};

	#define GEAR_COLLECTOR_MODES X(PREP_COLLECT) X(COLLECT) X(PREP_SCORE) X(SCORE)
	enum class Gear_collector_mode{
		#define X(NAME) NAME,
		GEAR_COLLECTOR_MODES
		#undef X
	};

	#define COLLECT_STEPS X(GEAR_COLLECTOR_DOWN) X(COLLECT)
	enum class Collect_step{
		#define X(NAME) NAME,
		COLLECT_STEPS
		#undef X
	};
	
	#define NO_COLLECT_STEPS X(BALL_COLLECTOR_IN) X(CLEAR_BALLS) X(GEAR_COLLECTOR_ACTIVE)
	enum class No_collect_step{
		#define X(NAME) NAME,
		NO_COLLECT_STEPS
		#undef X
	};

	#define TELEOP_ITEMS(X)\
		X(SINGLE_ARG(std::array<Nudge,NUDGES>),nudges) \
		X(Posedge_toggle,collect) \
		X(Posedge_trigger,collect_trigger) \
		X(Gear_collector_mode,gear_collector_mode) \
		X(Collect_step,collect_step) \
		X(No_collect_step,no_collect_step) \
		X(Countdown_timer,clear_ball_timer)
	STRUCT_MEMBERS(TELEOP_ITEMS)

	void collect_protocol(Toplevel::Status_detail const&,Toplevel::Goal&);
	void no_collect_protocol(Toplevel::Status_detail const&,const bool,const Time,Toplevel::Goal&);

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator<(Teleop const&)const;
	bool operator==(Teleop const&)const;
	void display(std::ostream&)const;
	Teleop();
	IMPL_STRUCT_DECLARE(Teleop,TELEOP_ITEMS)
};

std::ostream& operator<<(std::ostream&,Teleop::Nudge const&);
std::ostream& operator<<(std::ostream&,Teleop::Gear_collector_mode const&);
std::ostream& operator<<(std::ostream&,Teleop::Collect_step const&);
std::ostream& operator<<(std::ostream&,Teleop::No_collect_step const&);

bool operator<(Teleop::Nudge const&,Teleop::Nudge const&);
bool operator==(Teleop::Nudge const&,Teleop::Nudge const&);
double set_drive_speed(double,double,double);

#endif
