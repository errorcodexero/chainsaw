#ifndef SPEED_TRACKER_H
#define SPEED_TRACKER_H

#include "interface.h"

struct Speed_tracker{
	int last_ticks;
	Time last_time;

	double update(Time,int);
	Speed_tracker();
};

bool operator==(Speed_tracker const&,Speed_tracker const&);
bool operator!=(Speed_tracker const&,Speed_tracker const&);
bool operator<(Speed_tracker const&,Speed_tracker const&);
std::ostream& operator<<(std::ostream&,Speed_tracker const&);

#endif
