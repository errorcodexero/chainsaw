#include "intake.h"
#include <stdlib.h>

using namespace std;

#define FRONT_INTAKE_ADDRESS 3 //TODO: get real value
#define SIDES_INTAKE_ADDRESS 4 //TODO: get real value
#define FRONT_INTAKE_SPEED 1.0
#define SIDES_INTAKE_SPEED 1.0
#define STOP_SPEED 0.0

ostream& operator<<(ostream& o, Intake::Goal a){
	#define X(name) if(a==Intake::Goal::name)return o<<"Intake::Goal("#name")";
	X(IN) X(OUT) X(OFF)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Intake){ return o<<"Intake()";}

bool operator==(Intake::Output_applicator, Intake::Output_applicator){return 1;}

bool operator==(Intake a, Intake b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Intake a, Intake b){ return !(a==b);}

Robot_outputs Intake::Output_applicator::operator()(Robot_outputs r, Intake::Output out)const{
	switch(out){
		case Intake::Output::OUT:
			r.pwm[FRONT_INTAKE_ADDRESS]=-FRONT_INTAKE_SPEED;
			r.pwm[SIDES_INTAKE_ADDRESS]=-SIDES_INTAKE_SPEED;
			break;
		case Intake::Output::OFF:
			r.pwm[FRONT_INTAKE_ADDRESS]=STOP_SPEED;	
			r.pwm[SIDES_INTAKE_ADDRESS]=STOP_SPEED;	
			break;
		case Intake::Output::IN:
			r.pwm[SIDES_INTAKE_ADDRESS]=SIDES_INTAKE_SPEED;
			r.pwm[FRONT_INTAKE_ADDRESS]=FRONT_INTAKE_SPEED;
			break;
		default:
			assert(0);
	}
	return r;
}

Intake::Output Intake::Output_applicator::operator()(Robot_outputs r)const{
	if(r.pwm[SIDES_INTAKE_ADDRESS]==-SIDES_INTAKE_SPEED && r.pwm[FRONT_INTAKE_ADDRESS]==-FRONT_INTAKE_SPEED) return Intake::Output::OUT;
	if(r.pwm[SIDES_INTAKE_ADDRESS]==STOP_SPEED && r.pwm[FRONT_INTAKE_ADDRESS]==STOP_SPEED) return Intake::Output::OFF;
	if(r.pwm[SIDES_INTAKE_ADDRESS]==SIDES_INTAKE_SPEED && r.pwm[FRONT_INTAKE_ADDRESS]==FRONT_INTAKE_SPEED) return Intake::Output::IN;
	assert(0);
}
	
set<Intake::Goal> examples(Intake::Goal*){ 
	return {Intake::Goal::OUT,Intake::Goal::OFF,Intake::Goal::IN};
}

Intake::Output control(Intake::Status_detail, Intake::Goal goal){
	return goal;
}

bool ready(Intake::Status, Intake::Goal){ return 1;}

#ifdef INTAKE_TEST
#include "formal.h"

int main(){
	Intake a;
	tester(a);
}

#endif
