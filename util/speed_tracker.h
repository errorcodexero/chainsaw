#ifndef SPEED_TRACKER_H
#define SPEED_TRACKER_H

#include "interface.h"
#include "countdown_timer.h"

struct Speed_tracker{
	const Time POLL_TIME=0.05; //seconds

	int last_ticks;
	Time last_time;
	double speed;
	Countdown_timer poll_timer;

	double update(Time,int);
	Speed_tracker();
};

bool operator==(Speed_tracker const&,Speed_tracker const&);
bool operator!=(Speed_tracker const&,Speed_tracker const&);
bool operator<(Speed_tracker const&,Speed_tracker const&);
std::ostream& operator<<(std::ostream&,Speed_tracker const&);

#endif
