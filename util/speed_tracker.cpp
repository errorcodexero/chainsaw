#include "speed_tracker.h"

double Speed_tracker::update(Time t,int ticks){
	assert(t>last_time);
	
	int d_ticks=ticks-last_ticks;
	Time d_time=t-last_time;
	last_ticks=ticks;
	last_time=t;
	return d_ticks/d_time;
}

Speed_tracker::Speed_tracker():last_ticks(0),last_time(0){}

#ifdef SPEED_TRACKER_TEST
int main(){
	//TODO
}
#endif
