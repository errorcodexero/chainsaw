#include "climber.h"

static const int CLIMBER_PWM = 8;
static const float CLIMBER_POWER = .8;

#define HALL_EFFECT_DOWN_LOC 4 //TODO: get real value
#define HALL_EFFECT_UP_LOC 5 //TODO: get real value

Climber::Input::Input():down(true),up(false){}
Climber::Input::Input(bool a,bool b):down(a),up(b){}

Climber::Estimator::Estimator(){
	last = Status_detail::DOWN;
}

std::set<Climber::Goal> examples(Climber::Goal*){
	return {Climber::Goal::AUTO_DOWN, Climber::Goal::DOWN, Climber::Goal::UP, Climber::Goal::STOP, Climber::Goal::AUTO_UP};
}

std::set<Climber::Output> examples(Climber::Output*){
	return {Climber::Output::DOWN, Climber::Output::STOP, Climber::Output::UP};
}

std::set<Climber::Input> examples(Climber::Input*){
	return {
		{true,false},
		{false,false},
		{false,true},
		{true,true}//error case
	};
}

std::set<Climber::Status_detail> examples(Climber::Status_detail*){
	return {Climber::Status_detail::ERROR_,Climber::Status_detail::DOWN,Climber::Status_detail::GOING_DOWN,Climber::Status_detail::STOPPED,Climber::Status_detail::GOING_UP,Climber::Status_detail::UP};
}

std::ostream& operator<<(std::ostream& o,Climber::Goal g){
	#define X(name) if(g==Climber::Goal::name) return o<<"Climber::Goal("#name")";
	X(AUTO_DOWN) X(DOWN) X(UP) X(STOP) X(AUTO_UP)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Climber::Input a){
	return o<<"Input( down:"<<a.down<<" up:"<<a.up<<")";
}

std::ostream& operator<<(std::ostream& o,Climber::Status_detail a){
	#define X(STATUS) if(a==Climber::Status_detail::STATUS) return o<<""#STATUS;
	X(DOWN)
	X(GOING_DOWN)
	X(STOPPED)
	X(GOING_UP)
	X(UP)
	X(ERROR_)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o, Climber::Output a){
	#define X(OUT) if(a==Climber::Output::OUT) return o<<""#OUT;
	X(DOWN)
	X(STOP)
	X(UP)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Climber const&){
	return o<<"Climber()";
}

bool operator<(Climber::Input a,Climber::Input b){ 
	if(a.down && !b.down) return true;
	if(!a.up && b.up) return true;
	return false;
}
bool operator==(Climber::Input a,Climber::Input b){
	if(a.down != b.down) return false;
	return a.up == b.up;
}
bool operator!=(Climber::Input a, Climber::Input b){ return !(a==b); }

bool operator==(Climber::Estimator,Climber::Estimator){ return 1; }
bool operator!=(Climber::Estimator a, Climber::Estimator b){ return !(a==b); }

bool operator==(Climber,Climber){ return 1; }
bool operator!=(Climber a, Climber b){ return !(a==b); }

Climber::Input Climber::Input_reader::operator()(Robot_inputs const& r) const{
	return {r.digital_io.in[HALL_EFFECT_DOWN_LOC] == Digital_in::_1, r.digital_io.in[HALL_EFFECT_UP_LOC] == Digital_in::_1};
}

Robot_inputs Climber::Input_reader::operator()(Robot_inputs r, Climber::Input in) const{
	r.digital_io.in[HALL_EFFECT_DOWN_LOC] = in.down ? Digital_in::_1 : Digital_in::_0;
	r.digital_io.in[HALL_EFFECT_UP_LOC] = in.up ? Digital_in::_1 : Digital_in::_0;
	return r;
}

Robot_outputs Climber::Output_applicator::operator()(Robot_outputs r, Climber::Output o)const{
	if(o==Climber::Output::UP) r.pwm[CLIMBER_PWM] = CLIMBER_POWER;
	else if(o==Climber::Output::DOWN) r.pwm[CLIMBER_PWM] = -CLIMBER_POWER;
	else r.pwm[CLIMBER_PWM] = 0;
	return r;
}

Climber::Output Climber::Output_applicator::operator()(Robot_outputs const& r)const{
	if(r.pwm[CLIMBER_PWM] > 0) return Climber::Output::UP;
	if(r.pwm[CLIMBER_PWM] < 0) return Climber::Output::DOWN;
	return Climber::Output::STOP;
}

void Climber::Estimator::update(Time /*time*/,Climber::Input input,Climber::Output output){
	switch(output){
		case Climber::Output::UP:
			last = Status::GOING_UP;
			break;
		case Climber::Output::DOWN:
			last = Status::GOING_DOWN;
			break;
		case Climber::Output::STOP:
			last = Status::STOPPED;
			break;
		default:
			assert(0);
	}
	if(input.down) last = Status::DOWN;
	if(input.up) last = Status::UP;
	if(input.down && input.up) last = Status::ERROR_;
}

Climber::Status Climber::Estimator::get()const{
	return last;
}

Climber::Output control(Climber::Status status,Climber::Goal goal){
	switch(goal){
		case Climber::Goal::AUTO_DOWN:
			if(ready(status,goal)) return Climber::Output::STOP;
			return Climber::Output::DOWN;
		case Climber::Goal::DOWN:
			return Climber::Output::DOWN;
		case Climber::Goal::STOP:
			return Climber::Output::STOP;
		case Climber::Goal::UP:
			return Climber::Output::UP;
		case Climber::Goal::AUTO_UP:
			if(ready(status, goal)) return Climber::Output::STOP;
			return Climber::Output::UP;
		default:
			assert(0);
	}
}

Climber::Status status(Climber::Status s){
	return s;
}

bool ready(Climber::Status status,Climber::Goal goal){
	switch(goal){
		case Climber::Goal::AUTO_DOWN:
			return true;
		case Climber::Goal::DOWN:
			return status == Climber::Status::DOWN;
		case Climber::Goal::STOP:
			return status == Climber::Status::STOPPED;
		case Climber::Goal::UP:
			return true;
		case Climber::Goal::AUTO_UP:
			return status == Climber::Status::UP;
		default:
			assert(0);
	}
	return false;
}

#ifdef CLIMBER_TEST
#include "formal.h"
int main(){
	Climber c;
	tester(c);
}
#endif
