#include "speed_tracker.h"

using namespace std;

double Speed_tracker::update(Time t,int ticks){
	if(t==last_time) return 0;
	
	int d_ticks=ticks-last_ticks;
	Time d_time=t-last_time;
	last_ticks=ticks;
	last_time=t;
	return d_ticks/d_time;
}

Speed_tracker::Speed_tracker():last_ticks(0),last_time(0){}

bool operator==(Speed_tracker const& a,Speed_tracker const& b){
	return a.last_ticks==b.last_ticks && a.last_time==b.last_time;
}
bool operator!=(Speed_tracker const& a,Speed_tracker const& b){ return !(a==b); }
bool operator<(Speed_tracker const& a,Speed_tracker const& b){
	if(a.last_time<b.last_time) return 1;
	if(b.last_time<a.last_time) return 0;
	if(a.last_ticks<b.last_ticks) return 1;
	if(b.last_ticks<a.last_ticks) return 0;
	return 0;
}
ostream& operator<<(ostream& o,Speed_tracker const& a){
	o<<"Speed_tracker(";
	o<<"last_ticks:"<<a.last_ticks;
	o<<" last_time:"<<a.last_time;
	return o<<")";
}

#ifdef SPEED_TRACKER_TEST
int main(){
	//TODO
}
#endif
