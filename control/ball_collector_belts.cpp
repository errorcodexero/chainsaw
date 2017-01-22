#include "ball_collector_belts.h"

using namespace std;

#define T Ball_collector_belts

set<T::Output> examples(const T::Output*){
	#define X(A) T::Output::A,
	return {BALL_COLLECTOR_BELTS_OUTPUTS(X)};
	#undef X
}

static const int BELT_IO=7;

Robot_outputs T::Output_applicator::operator()(Robot_outputs robot_outputs,Output output)const{
	robot_outputs.pwm[BELT_IO]=[=](){
		switch(output){
			case Output::IN: return 1;
			case Output::OUT: return -1;
			case Output::STOP: return 0;
			default: assert(0);
		}
	}();
	return robot_outputs;
}

T::Output T::Output_applicator::operator()(Robot_outputs robot_outputs)const{
	auto v=robot_outputs.pwm[BELT_IO];
	if(v>0) return Output::IN;
	if(v<0) return Output::OUT;
	return Output::STOP;
}

ostream& operator<<(ostream& o,T const&){
	return o<<"Ball_collector_belts(...)";
}

ostream& operator<<(ostream& o,T::Output a){
	#define X(A) if(T::Output::A==a) return o<<""#A;
	BALL_COLLECTOR_BELTS_OUTPUTS(X)
	#undef X
	assert(0);
}

Ball_collector_belts::Output control(Ball_collector_belts::Status_detail,Ball_collector_belts::Goal goal){
	return goal;
}

bool ready(Ball_collector_belts::Status,Ball_collector_belts::Goal){
	return 1;
}

#ifdef BALL_COLLECTOR_BELTS_TEST
#include "formal.h"

int main(){
	Ball_collector_belts g;
	tester(g);
}
#endif
