#include "gear_shifter.h"

using namespace std;

#define R_SHIFTER_SOLENOID 0
#define L_SHIFTER_SOLENOID 1

Robot_outputs Gear_shifter::Output_applicator::operator()(Robot_outputs robot,Output out)const{
	robot.solenoid[R_SHIFTER_SOLENOID]=out==Output::HIGH;
	robot.solenoid[L_SHIFTER_SOLENOID]=out==Output::HIGH;
	return robot;
}

Gear_shifter::Output Gear_shifter::Output_applicator::operator()(Robot_outputs robot)const{
	//Assumes that both solenoids are set to the same value
	return robot.solenoid[R_SHIFTER_SOLENOID]?Output::HIGH:Output::LOW;
}

Gear_shifter::Estimator::Estimator(){}

Gear_shifter::Status_detail Gear_shifter::Estimator::get()const{
	return {};
}

void Gear_shifter::Estimator::update(Time,Input,Output){

}

ostream& operator<<(ostream& o,Gear_shifter::Goal const& g){
	#define X(A) if(g==Gear_shifter::Goal::A) return o<<"Gear_shifter::Goal("#A")";
	GEAR_SHIFTER_GOALS
	#undef X
	assert(0); 
}

ostream& operator<<(ostream& o,Gear_shifter::Output const& out){
	#define X(A) if(out==Gear_shifter::Output::A) return o<<"Gear_shifter::Output("#A")";
	GEAR_SHIFTER_OUTPUTS
	#undef X
	assert(0); 
}

ostream& operator<<(ostream& o,Gear_shifter const&){
	o<<"Gear_shifter(";
	//TODO
	return o<<")";
}

set<Gear_shifter::Goal> examples(Gear_shifter::Goal*){
	#define X(A) Gear_shifter::Goal::A,
	return {GEAR_SHIFTER_GOALS};
	#undef X
}

set<Gear_shifter::Output> examples(Gear_shifter::Output*){
	#define X(A) Gear_shifter::Output::A,
	return {GEAR_SHIFTER_OUTPUTS};
	#undef X
}

Gear_shifter::Status status(Gear_shifter::Status_detail){
	return {};
}

Gear_shifter::Output control(Gear_shifter::Status_detail status,Gear_shifter::Goal goal){
	switch(goal) {
		case Gear_shifter::Goal::AUTO: return status;
		case Gear_shifter::Goal::LOW: return Gear_shifter::Output::LOW;
		case Gear_shifter::Goal::HIGH: return Gear_shifter::Output::HIGH;
		default: assert(0);
	}
}

bool ready(Gear_shifter::Status,Gear_shifter::Goal){
	return 1;
}

#ifdef GEAR_SHIFTER_TEST
#include "formal.h"
int main(){
	Gear_shifter g;
	tester(g);
}
#endif
