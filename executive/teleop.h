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

	#define GEAR_SCORE_STEPS X(CLEAR_BALLS) X(RAISE_ARM) X(LIFT_GEAR)
	enum class Gear_score_step{
		#define X(NAME) NAME,
		GEAR_SCORE_STEPS
		#undef X
	};

	#define TELEOP_ITEMS(X)\
		X(SINGLE_ARG(std::array<Nudge,NUDGES>),nudges) \
		X(Countdown_timer,clear_ball_timer) \
		X(Gear_collector_mode,gear_collector_mode) \
		X(Gear_score_step,gear_score_step) \
		X(Posedge_toggle,collect) \
		X(int,print_num)
	STRUCT_MEMBERS(TELEOP_ITEMS)

	void gear_score_protocol(Toplevel::Status_detail const&,const bool,const Time,Toplevel::Goal&);

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
std::ostream& operator<<(std::ostream&,Teleop::Gear_score_step const&);

bool operator<(Teleop::Nudge const&,Teleop::Nudge const&);
bool operator==(Teleop::Nudge const&,Teleop::Nudge const&);
double set_drive_speed(double,double,double);

#endif
