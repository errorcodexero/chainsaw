#include "auto_guided_turn.h"
#include "teleop.h"

using namespace std;

double turn_profile(const double DIFF){
	static const double SCALE_FACTOR = 3.0;//arbitary, TODO
	static const double MIN = -1.0, MAX = 1.0;
	return clamp(DIFF*SCALE_FACTOR,MIN,MAX);
}

Executive Auto_guided_turn::next_mode(Next_mode_info info){
	if(true){//TODO	
		in_range.update(info.in.now,info.in.robot_mode.enabled);
		cout <<"IN  RANGE"<< in_range << "\n";
	} else {
		const double IN_RANGE_TIME = 2.0;//seconds
		in_range.set(IN_RANGE_TIME);
	}
	if(in_range.done()){
		return Executive{Teleop()};
	}
	return Executive{Auto_guided_turn(CONSTRUCT_STRUCT_PARAMS(AUTO_GUIDED_TURN_ITEMS))};
}

Toplevel::Goal Auto_guided_turn::run(Run_info info){
	Toplevel::Goal goals;
	(void)info;
	double power = turn_profile(0.0);//TODO
	goals.drive.left = -power;
	goals.drive.right = power;
	return goals;
}

bool Auto_guided_turn::operator==(Auto_guided_turn const&)const{ return true; }//TODO: update with values in that struct

#ifdef AUTO_GUIDED_TURN_TEST
#include "test.h"

int main(){
	Auto_guided_turn a;
	test_executive(a);
}
#endif

