#include "ball_lifter.h"
#include <stdlib.h>

using namespace std;

#define BALL_LIFTER_ADDRESS 6
#define BALL_LIFTER_SPEED -.40 //TODO: tune this //TODO make this nicer

ostream& operator<<(ostream& o, Ball_lifter::Goal a){
	#define X(name) if(a==Ball_lifter::Goal::name)return o<<"Ball_lifter::Goal("#name")";
	X(UP) X(DOWN) X(OFF)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Ball_lifter){ return o<<"Ball_lifter()";}

bool operator==(Ball_lifter::Output_applicator, Ball_lifter::Output_applicator){return 1;}

bool operator==(Ball_lifter a, Ball_lifter b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Ball_lifter a, Ball_lifter b){ return !(a==b);}


Robot_outputs Ball_lifter::Output_applicator::operator()(Robot_outputs r, Ball_lifter::Output out)const{
	switch(out){
		case Ball_lifter::Output::DOWN:
			r.pwm[BALL_LIFTER_ADDRESS]=-BALL_LIFTER_SPEED;
			break;
		case Ball_lifter::Output::OFF:
			r.pwm[BALL_LIFTER_ADDRESS]=0.0;	
			break;
		case Ball_lifter::Output::UP:
			r.pwm[BALL_LIFTER_ADDRESS]=BALL_LIFTER_SPEED;
			break;
		default:
			assert(0);
	}
	return r;
}

Ball_lifter::Output Ball_lifter::Output_applicator::operator()(Robot_outputs r)const{
	if(r.pwm[BALL_LIFTER_ADDRESS] < 0.0) return Ball_lifter::Output::DOWN;
	if(r.pwm[BALL_LIFTER_ADDRESS] == 0.0) return Ball_lifter::Output::OFF;
	if(r.pwm[BALL_LIFTER_ADDRESS] > 0.0) return Ball_lifter::Output::UP;
	assert(0);
}
	
set<Ball_lifter::Goal> examples(Ball_lifter::Goal*){ 
	return {Ball_lifter::Goal::DOWN,Ball_lifter::Goal::OFF,Ball_lifter::Goal::UP};
}

Ball_lifter::Output control(Ball_lifter::Status_detail, Ball_lifter::Goal goal){
	return goal;
}

bool ready(Ball_lifter::Status, Ball_lifter::Goal){ return 1;}

#ifdef BALL_LIFTER_TEST
#include "formal.h"

int main(){
	Ball_lifter a;
	tester(a);
}
#endif
