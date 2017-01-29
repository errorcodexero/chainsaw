#include "speed_tracker.h"

using namespace std;

double Speed_tracker::update(Time t,int ticks){
	poll_timer.update(t,1);
	if(poll_timer.done()){
		speed=(ticks-last_ticks)/(t-last_time);
		last_ticks=ticks;
		last_time=t;
		poll_timer.set(POLL_TIME);
	}
	return speed;
}

Speed_tracker::Speed_tracker():last_ticks(0),last_time(0),speed(0){
	poll_timer.set(POLL_TIME);
}

bool operator==(Speed_tracker const& a,Speed_tracker const& b){
	return a.last_ticks==b.last_ticks && a.last_time==b.last_time && a.speed==b.speed && a.poll_timer==b.poll_timer;
}
bool operator!=(Speed_tracker const& a,Speed_tracker const& b){ return !(a==b); }
bool operator<(Speed_tracker const& a,Speed_tracker const& b){
	if(a.last_time<b.last_time) return 1;
	if(b.last_time<a.last_time) return 0;
	if(a.last_ticks<b.last_ticks) return 1;
	if(b.last_ticks<a.last_ticks) return 0;
	if(a.speed<b.speed) return 1;
	if(b.speed<a.speed) return 0;
	if(a.poll_timer<b.poll_timer) return 1;
	if(b.poll_timer<a.poll_timer) return 0;
	return 0;
}
ostream& operator<<(ostream& o,Speed_tracker const& a){
	o<<"Speed_tracker(";
	o<<"last_ticks:"<<a.last_ticks;
	o<<" last_time:"<<a.last_time;
	o<<" speed:"<<a.speed;
	o<<" poll_timer:"<<a.poll_timer;
	return o<<")";
}

#ifdef SPEED_TRACKER_TEST
int main(){
	//TODO
}
#endif
