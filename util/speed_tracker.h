#ifndef SPEED_TRACKER_H
#define SPEED_TRACKER_H

#include "interface.h"

class Speed_tracker{
	int last_ticks;
	Time last_time;

	public:
	double update(Time,int);
	Speed_tracker();
};

#endif
