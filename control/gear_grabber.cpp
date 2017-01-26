#include "gear_grabber.h"

using namespace std;

#define HALL_EFFECT_LOC 7 //sensor not on robot
#define PISTON_LOC 6 

Gear_grabber::Input::Input():closed(false),enabled(false){}
Gear_grabber::Input::Input(bool a,bool b):closed(a),enabled(b){}

Gear_grabber::Estimator::Estimator():last(Gear_grabber::Status_detail::OPEN){
	const Time OPEN_TIME = .2, CLOSE_TIME = .2;
	open_timer.set(OPEN_TIME);
	close_timer.set(CLOSE_TIME);
}

bool operator==(const Gear_grabber::Input a,const Gear_grabber::Input b){
	if(a.closed != b.closed) return false;
	return a.enabled == b.enabled;
}

bool operator!=(const Gear_grabber::Input a,const Gear_grabber::Input b){
	return !(a==b);
}

bool operator<(const Gear_grabber::Input a,const Gear_grabber::Input b){
	if(a.closed && !b.closed) return false;
	return !a.enabled && b.enabled;
}

bool operator==(const Gear_grabber::Estimator a,const Gear_grabber::Estimator b){
	if(a.last != b.last) return false;
	return a.open_timer == b.open_timer;
}

bool operator!=(const Gear_grabber::Estimator a,const Gear_grabber::Estimator b){
	return !(a==b);
}

ostream& operator<<(ostream& o, const Gear_grabber::Estimator a){
	return o<<"Estimator(last:"<<a.last<<" open_timer:"<<a.open_timer<<")";
}

ostream& operator<<(ostream& o,const Gear_grabber::Input a){
	return o<<" Input( closed:"<<a.closed<<" enabled:"<<a.enabled<<")";
}

ostream& operator<<(ostream& o,const Gear_grabber::Goal a){
	#define X(GOAL) if(a==Gear_grabber::Goal::GOAL) return o<<""#GOAL;
	X(OPEN) X(CLOSE)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,const Gear_grabber::Status_detail a){
	#define X(STATUS) if(a==Gear_grabber::Status_detail::STATUS) return o<<""#STATUS;
	X(OPEN)
	X(OPENING)
	X(CLOSING)
	X(CLOSED)
	#undef X
	assert(0);
}

set<Gear_grabber::Input> examples(Gear_grabber::Input*){
	return {
		{false,false},
		{true,false},
		{false,true},
		{true,true}
	};
}

set<Gear_grabber::Goal> examples(Gear_grabber::Goal*){
	return {Gear_grabber::Goal::OPEN,Gear_grabber::Goal::CLOSE};
}

set<Gear_grabber::Status_detail> examples(Gear_grabber::Status_detail*){
	return {Gear_grabber::Status_detail::OPEN,Gear_grabber::Status_detail::OPENING,Gear_grabber::Status_detail::CLOSING,Gear_grabber::Status_detail::CLOSED};
}

ostream& operator<<(ostream& o,Gear_grabber){
	return o<<"Gear_grabber()";
}

Gear_grabber::Status_detail Gear_grabber::Estimator::get()const{
	return last;
}

void Gear_grabber::Estimator::update(Time time,Input input,Output output){
	switch(output){
		case Gear_grabber::Output::OPEN:
			if(last == Gear_grabber::Status_detail::OPENING){
				open_timer.update(time,input.enabled);
			} else if(last != Gear_grabber::Status_detail::OPEN){
				const Time OPEN_TIME = .2;//seconds .tested 
				open_timer.set(OPEN_TIME);
				last = Status_detail::OPENING;
			}
			if(open_timer.done()){
				 last = Gear_grabber::Status_detail::OPEN;
			}
			break;
		case Gear_grabber::Output::CLOSE:
			if(last == Gear_grabber::Status_detail::CLOSING){
				close_timer.update(time,input.enabled);
			} else if(last != Gear_grabber::Status_detail::CLOSED){
				const Time CLOSE_TIME = .2;//seconds tested
				close_timer.set(CLOSE_TIME);
				last = Status_detail::CLOSING;
			}
			if(close_timer.done()){
				last = Gear_grabber::Status_detail::CLOSED;
			}
			//if(input.closed) last = Gear_grabber::Status_detail::CLOSED;
			//else last = Gear_grabber::Status_detail::CLOSING;
			break;
		default:
			assert(0);
	}
	
}

Gear_grabber::Output control(Gear_grabber::Status /*status*/, Gear_grabber::Goal goal){
	return goal;
}

Robot_outputs Gear_grabber::Output_applicator::operator()(Robot_outputs r,Output out)const{
	r.solenoid[PISTON_LOC] = out == Gear_grabber::Output::OPEN; 
	return r;
}

Gear_grabber::Output Gear_grabber::Output_applicator::operator()(Robot_outputs r)const{
	return r.solenoid[PISTON_LOC] ? Gear_grabber::Output::OPEN : Gear_grabber::Output::CLOSE;
}

Robot_inputs Gear_grabber::Input_reader::operator()(Robot_inputs r,Input in)const{
	r.digital_io.in[HALL_EFFECT_LOC] = in.closed ? Digital_in::_1 : Digital_in::_0;
	r.robot_mode.enabled = in.enabled;
	return r;
}

Gear_grabber::Input Gear_grabber::Input_reader::operator()(Robot_inputs r)const{
	return {(r.digital_io.in[HALL_EFFECT_LOC] == Digital_in::_1),r.robot_mode.enabled};
}

Gear_grabber::Status status(Gear_grabber::Status_detail status_detail){
	return status_detail;
}

bool ready(Gear_grabber::Status status,Gear_grabber::Goal goal){
	switch(goal){
		case Gear_grabber::Goal::OPEN:
			return status == Gear_grabber::Status::OPEN;
		case Gear_grabber::Goal::CLOSE:
			return status == Gear_grabber::Status::CLOSED;
		default:
			assert(0);
	}
	
	return false;
}

#ifdef GEAR_GRABBER_TEST
#include "formal.h"

int main(){
	{
		Gear_grabber g;
		tester(g);
	}
	{
		Gear_grabber g;
		Gear_grabber::Goal goal = Gear_grabber::Goal::CLOSE;
		const bool ENABLED = true;
		const Time CLOSE_TIME = 5;//seconds - the amount of time before the simulated hall effect reads that it's closed
		for(Time t: range(1000)){
			bool closed = t >= CLOSE_TIME;
			Gear_grabber::Status_detail status = g.estimator.get();
			Gear_grabber::Output out = control(status,goal);
			
			cout<<"t:"<<t<<"\tgoal:"<<goal<<"\tstatus:"<<status<<"\n";
			
			g.estimator.update(t,Gear_grabber::Input{closed,ENABLED},out);
			if(ready(status,goal)){
				cout<<"Goal "<<goal<<" reached. Finishing.\n";
				break;
			}
		}

		goal = Gear_grabber::Goal::OPEN;

		for(Time t: range(1000)){
			bool closed = g.estimator.get() == Gear_grabber::Status_detail::CLOSED;
			Gear_grabber::Status_detail status = g.estimator.get();
			Gear_grabber::Output out = control(status,goal);
			
			cout<<"t:"<<t<<"\tgoal:"<<goal<<"\tstatus:"<<status<<"\n";
			
			g.estimator.update(t,Gear_grabber::Input{closed,ENABLED},out);
			if(ready(status,goal)){
				cout<<"Goal "<<goal<<" reached. Finishing.\n";
				break;
			}
		}
	}
}
#endif
