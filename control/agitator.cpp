#include "agitator.h"
#include "gear_shifter.h"

using namespace std;

ostream& operator<<(ostream& o,Agitator const&){
	return o<<"Agitator(...)";
}

Agitator::Output control(Agitator::Status_detail,Agitator::Goal g){
	return g;
}

bool ready(Agitator::Status,Agitator::Goal){ return 1; }

static const int AGITATOR_PWM=3;

Robot_outputs Agitator::Output_applicator::operator()(Robot_outputs robot_outputs,Output output)const{
	robot_outputs.pwm[AGITATOR_PWM]=output;
	return robot_outputs;
}

Agitator::Output Agitator::Output_applicator::operator()(Robot_outputs robot_outputs)const{
	auto v=robot_outputs.pwm[AGITATOR_PWM];
	return v>0;
}

#ifdef AGITATOR_TEST
#include "formal.h"

int main(){
	Agitator a;
	tester(a);
}
#endif
