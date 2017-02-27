#include "climber.h"

static const int CLIMBER_PWM = 8;
static const float CLIMBER_POWER = 1.0;

using namespace std;

/*
#define HALL_EFFECT_CLIMBED_LOC 4 //TODO: get real value
#define HALL_EFFECT_RELEASED_LOC 5 //TODO: get real value

Climber::Input::Input():climbed(true),released(false){}
Climber::Input::Input(bool a,bool b):climbed(a),released(b){}
*/

Climber::Estimator::Estimator():last(Status_detail::STOPPED),last_output(Output::STOP),state_timer(){}

set<Climber::Goal> examples(Climber::Goal*){
	return {Climber::Goal::AUTO_CLIMB, Climber::Goal::CLIMB, Climber::Goal::RELEASE, Climber::Goal::STOP, Climber::Goal::AUTO_RELEASE};
}

set<Climber::Output> examples(Climber::Output*){
	return {Climber::Output::RELEASE, Climber::Output::STOP, Climber::Output::CLIMB};
}
/*
set<Climber::Input> examples(Climber::Input*){
	return {
		{true,false},
		{false,false},
		{false,true},
		{true,true}//error case
	};
}
*/
set<Climber::Status_detail> examples(Climber::Status_detail*){
	return {Climber::Status_detail::ERROR_,Climber::Status_detail::RELEASED,Climber::Status_detail::RELEASING,Climber::Status_detail::STOPPED,Climber::Status_detail::CLIMBING,Climber::Status_detail::CLIMBED};
}

ostream& operator<<(ostream& o,Climber::Goal g){
	#define X(name) if(g==Climber::Goal::name) return o<<""#name;
	X(AUTO_CLIMB) X(CLIMB) X(RELEASE) X(STOP) X(AUTO_RELEASE)
	#undef X
	assert(0);
}
/*
ostream& operator<<(ostream& o,Climber::Input a){
	return o<<"Input(climbed:"<<a.climbed<<" released:"<<a.released<<")";
}
*/
ostream& operator<<(ostream& o,Climber::Status_detail a){
	#define X(STATUS) if(a==Climber::Status_detail::STATUS) return o<<""#STATUS;
	X(CLIMBED)
	X(CLIMBING)
	X(STOPPED)
	X(RELEASING)
	X(RELEASED)
	X(ERROR_)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Climber::Output a){
	#define X(OUT) if(a==Climber::Output::OUT) return o<<""#OUT;
	X(CLIMB)
	X(STOP)
	X(RELEASE)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Climber const&){
	return o<<"Climber()";
}
/*
bool operator<(Climber::Input a,Climber::Input b){ 
	if(a.released && !b.released) return true;
	if(!a.climbed && b.climbed) return true;
	return false;
}

bool operator==(Climber::Input a,Climber::Input b){
	if(a.climbed != b.climbed) return false;
	return a.released == b.released;
}
bool operator!=(Climber::Input a, Climber::Input b){ return !(a==b); }
*/
bool operator==(Climber::Estimator const a,Climber::Estimator const b){
	return a.last == b.last && a.last_output == b.last_output && a.state_timer == b.state_timer;
}
bool operator!=(Climber::Estimator a, Climber::Estimator b){ return !(a==b); }

bool operator==(Climber,Climber){ return 1; }
bool operator!=(Climber a, Climber b){ return !(a==b); }
/*
Climber::Input Climber::Input_reader::operator()(Robot_inputs const& r) const{
	return {r.digital_io.in[HALL_EFFECT_CLIMBED_LOC] == Digital_in::_1, r.digital_io.in[HALL_EFFECT_RELEASED_LOC] == Digital_in::_1};
}

Robot_inputs Climber::Input_reader::operator()(Robot_inputs r, Climber::Input in) const{
	r.digital_io.in[HALL_EFFECT_CLIMBED_LOC] = in.climbed ? Digital_in::_1 : Digital_in::_0;
	r.digital_io.in[HALL_EFFECT_RELEASED_LOC] = in.released ? Digital_in::_1 : Digital_in::_0;
	return r;
}
*/
Robot_outputs Climber::Output_applicator::operator()(Robot_outputs r, Climber::Output o)const{
	if(o==Climber::Output::CLIMB) r.pwm[CLIMBER_PWM] = -CLIMBER_POWER;
	else if(o==Climber::Output::RELEASE) r.pwm[CLIMBER_PWM] = CLIMBER_POWER;
	else r.pwm[CLIMBER_PWM] = 0;
	return r;
}

Climber::Output Climber::Output_applicator::operator()(Robot_outputs const& r)const{
	if(r.pwm[CLIMBER_PWM] < 0) return Climber::Output::CLIMB;
	if(r.pwm[CLIMBER_PWM] > 0) return Climber::Output::RELEASE;
	return Climber::Output::STOP;
}

void Climber::Estimator::update(Time now,Climber::Input /*input*/,Climber::Output output){
	static const Time FULL_STATE_SWITCH_TIME = 2.0;//seconds, assumed
	if(output != last_output){
		state_timer.set(FULL_STATE_SWITCH_TIME);
	} else{
		state_timer.update(now,true);
	}
	switch(output){
		case Climber::Output::CLIMB:
			if(state_timer.done()) last = Status::CLIMBED;
			else last = Status::CLIMBING;
			break;
		case Climber::Output::RELEASE:
			if(state_timer.done()) last = Status::RELEASED;
			else last = Status::RELEASING;
			break;
		case Climber::Output::STOP:
			if(last != Status::CLIMBED && last != Status::RELEASED) last = Status::STOPPED;
			break;
		default:
			assert(0);
	}
	//if(input.climbed) last = Status::CLIMBED;
	//if(input.released) last = Status::RELEASED;
	//if(input.climbed && input.released) last = Status::ERROR_;
	last_output = output;
}

Climber::Status Climber::Estimator::get()const{
	return last;
}

Climber::Output control(Climber::Status status,Climber::Goal goal){
	switch(goal){
		case Climber::Goal::AUTO_CLIMB:
			if(ready(status,goal)) return Climber::Output::STOP;
			return Climber::Output::CLIMB;
		case Climber::Goal::CLIMB:
			return Climber::Output::CLIMB;
		case Climber::Goal::STOP:
			return Climber::Output::STOP;
		case Climber::Goal::RELEASE:
			return Climber::Output::RELEASE;
		case Climber::Goal::AUTO_RELEASE:
			if(ready(status, goal)) return Climber::Output::STOP;
			return Climber::Output::RELEASE;
		default:
			assert(0);
	}
}

Climber::Status status(Climber::Status s){
	return s;
}

bool ready(Climber::Status status,Climber::Goal goal){
	switch(goal){
		case Climber::Goal::CLIMB:
			return status == Climber::Status::CLIMBING || status == Climber::Status::CLIMBED;
		case Climber::Goal::AUTO_CLIMB:
			return status == Climber::Status::CLIMBED;
		case Climber::Goal::STOP:
			return status == Climber::Status::STOPPED;
		case Climber::Goal::RELEASE:
			return status == Climber::Status::RELEASING || status == Climber::Status::RELEASED;
		case Climber::Goal::AUTO_RELEASE:
			return status == Climber::Status::RELEASED;
		default:
			return 1;
			//assert(0);
	}
	return false;
}

#ifdef CLIMBER_TEST
#include "formal.h"
int main(){
	{
		Climber c;
		tester(c);
	}
	for(Climber::Goal goal: examples((Climber::Goal*)nullptr)){
		cout<<"\nTEST\n";
		Climber c;

		bool print_ready = true;
		int i = 0;//stop if ready for a certain number of iterations

		for(Time t = 0.0; t < 100; t += 0.5){
			Climber::Status status = c.estimator.get();
			Climber::Output output = control(status,goal);
		
			cout<<"\t"<<t<<"\tgoal:"<<goal<<"    status:"<<status<<"    output:"<<output<<"\n";
			
			c.estimator.update(t,{},output);
			
			if(ready(status,goal)){
				if(print_ready){
					cout<<"\tREADY\n";
					print_ready = false;
				}
				if(i > 5) break;
				i++;
			}
		}	
	}
}
#endif
