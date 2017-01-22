#include "gear_grasper.h"

using namespace std;

std::ostream& operator<<(ostream& o,Gear_grasper const& ){
	o<<"Gear_grasper(";
	o<<"...";
	return o<<")";
}

std::set<Gear_grasper::Goal> examples(const Gear_grasper::Goal*){
	#define X(A) Gear_grasper::Goal::A,
	return {GEAR_GRASPER_GOALS(X)};
	#undef X
}

std::ostream& operator<<(std::ostream& o,Gear_grasper::Goal a){
	#define X(A) if(a==Gear_grasper::Goal::A) return o<<""#A;
	GEAR_GRASPER_GOALS(X)
	#undef X
	assert(0);
}

std::set<Gear_grasper::Output> examples(const Gear_grasper::Output*){
	#define X(A) Gear_grasper::Output::A,
	return {GEAR_GRASPER_OUTPUTS(X)};
	#undef X
}

std::ostream& operator<<(std::ostream& o,Gear_grasper::Output a){
	#define X(A) if(a==Gear_grasper::Output::A) return o<<""#A;
	GEAR_GRASPER_OUTPUTS(X)
	#undef X
	assert(0);
}

static const int GEAR_GRASPER_SOLENOID=4;//TOTALLY MADE UP

Robot_outputs Gear_grasper::Output_applicator::operator()(Robot_outputs robot_outputs,Output output)const{
	robot_outputs.solenoid[GEAR_GRASPER_SOLENOID]=(output==Gear_grasper::Output::OPEN);
	return robot_outputs;
}

Gear_grasper::Output Gear_grasper::Output_applicator::operator()(Robot_outputs robot_outputs)const{
	return robot_outputs.solenoid[GEAR_GRASPER_SOLENOID]?Gear_grasper::Output::OPEN:Gear_grasper::Output::CLOSE;
}

Gear_grasper::Output control(Gear_grasper::Status_detail,Gear_grasper::Goal goal){
	switch(goal){
		case Gear_grasper::Goal::OPEN:
			return Gear_grasper::Output::OPEN;
		case Gear_grasper::Goal::CLOSE:
			return Gear_grasper::Output::CLOSE;
		case Gear_grasper::Goal::X:
			switch(status){
			case Gear_grasper::Status::OPEN:
				case Gear_grasper::Status::OPENING:
					return Gear_grasper::Output::OPEN;
				case Gear_grasper::Status::CLOSED:
				case Gear_grasper::Status::CLOSING:
					return Gear_grasper::Output::CLOSE;
				default:
					assert(0);
			}
		default:
			assert(0);
	}
}

bool ready(Gear_grasper::Status status,Gear_grasper::Goal goal){
	switch(goal){
		case Gear_grasper::Goal::OPEN:
			return status==Gear_grasper::Status::OPEN;
		case Gear_grasper::Goal::CLOSE:
			return status==Gear_grasper::Status::CLOSE;
		case Gear_grasper::Goal::X:
			return 1;
		default:
			assert(0);
	}
}

set<Gear_grasper::Status_detail> examples(const Gear_grasper::Status_detail*){
	#define X(A) Gear_grasper::Status_detail::A,
	return {GEAR_GRASPER_STATUSES(X)};
	#undef X
}

Gear_grasper::Status status(Gear_grasper::Status_detail a){
	return a;
}

ostream& operator<<(ostream& o,Gear_grasper::Status_detail const& a){
	#define X(A) if(a==Gear_grasper::Status_detail::A) return o<<""#A;
	GEAR_GRASPER_STATUSES(X)
	#undef X
	assert(0);
}

Gear_grasper::Estimator::Estimator():
	last_output(Gear_grasper::Output::CLOSE)
{}

Gear_grasper::Estimator::update(Time now,Input,Output output){
	until_finish.update(now,1);
	if(output!=last_output){
		until_finish.set(.3);//time to open or close, totally made up.
	}
	last_output=output;
}

Gear_grasper::Estimator::get()const{
	if(until_finish.done()){
		if(last_output==Output::OPEN){
			return Status::OPEN;
		}
		return Status::CLOSED;
	}
	if(last_output==Output::OPEN){
		return Status::OPENING;
	}
	return Status::CLOSING;
}

#ifdef GEAR_GRASPER_TEST
#include "formal.h"
int main(){
	Tester_mode tester_mode;
	//tester_mode.check_multiple_outputs=0;
	Gear_grasper g;
	tester(g,tester_mode);
}
#endif
