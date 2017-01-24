#include "ball_collector_lift.h"

using namespace std;

#define T Ball_collector_lift

Ball_collector_lift::Estimator::Estimator():
	last_output(T::Output::UP)
{}

void Ball_collector_lift::Estimator::update(Time time,Input,Output output){
	timer.update(1,time);
	if(output!=last_output){
		timer.set(1.2);//totally made up amount of time; this will probably be different for going up or down.
	}
	last_output=output;
}

T::Status_detail T::Estimator::get()const{
	if(timer.done()){
		if(last_output==Output::UP){
			return Status_detail::UP;
		}
		return Status_detail::DOWN;
	}
	if(last_output==Output::UP){
		return Status::GOING_UP;
	}
	return Status::GOING_DOWN;
}

T::Status status(T::Status_detail a){
	return a;
}

bool ready(T::Status status,T::Goal goal){
	switch(goal){
		case T::Goal::UP:
			return status==T::Status::UP;
		case T::Goal::DOWN:
			return status==T::Status::DOWN;
		case T::Goal::X:
			return 1;
		default:
			assert(0);
	}
}

set<T::Status_detail> examples(const T::Status_detail*){
	#define X(A) T::Status_detail::A,
	return {BALL_COLLECTOR_LIFT_STATUSES(X)};
	#undef X
}

ostream& operator<<(ostream& o,T::Goal a){
	#define X(A) if(a==T::Goal::A) return o<<""#A;
	BALL_COLLECTOR_LIFT_GOALS(X)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,T::Output a){
	#define X(A) if(T::Output::A==a) return o<<""#A;
	BALL_COLLECTOR_LIFT_OUTPUTS(X)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,T::Status_detail a){
	#define X(A) if(T::Status_detail::A==a) return o<<""#A;
	BALL_COLLECTOR_LIFT_STATUSES(X)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,T const&){
	return o<<"Ball_collector_lift(...)";
}

set<T::Goal> examples(const T::Goal*){
	#define X(A) T::Goal::A,
	return {BALL_COLLECTOR_LIFT_GOALS(X)};
	#undef X
}

T::Output control(T::Status_detail status,T::Goal goal){
	switch(goal){
		case T::Goal::UP:
			return T::Output::UP;
		case T::Goal::DOWN:
			return T::Output::DOWN;
		case T::Goal::X:
			switch(status){
				case T::Status::UP:
				case T::Status::GOING_UP:
					return T::Output::UP;
				case T::Status::DOWN:
				case T::Status::GOING_DOWN:
					return T::Output::DOWN;
				default:
					assert(0);
			}
		default:
			assert(0);
	}
}

set<T::Output> examples(const T::Output*){
	#define X(A) T::Output::A,
	return {BALL_COLLECTOR_LIFT_OUTPUTS(X)};
	#undef X
	assert(0);
}

static const int BALL_COLLECTOR_LIFT_SOLENOID=2;

T::T():output_applicator(BALL_COLLECTOR_LIFT_SOLENOID){}

T::Output_applicator::Output_applicator(int i):
	solenoid(i) //could check that this is a reasonable number.
{}

Robot_outputs T::Output_applicator::operator()(Robot_outputs robot_outputs,T::Output output)const{
	robot_outputs.solenoid[solenoid]=(output==Output::DOWN);
	return robot_outputs;
}

T::Output T::Output_applicator::operator()(Robot_outputs robot_outputs)const{
	return robot_outputs.solenoid[solenoid]?Output::DOWN:Output::UP;
	nyi
}

#ifdef BALL_COLLECTOR_LIFT_TEST
#include "formal.h"
int main(){
	Ball_collector_lift g;
	tester(g);
}
#endif
