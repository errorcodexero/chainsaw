#include "shooter.h"
#include <cmath>
#include <cassert>

#define SHOOTER_LOC 0 //actuall one, but we account for the fact that CAN doesn't start at zero in talon_srx_controls. this is in trello as a todo

#define SHOOTER_POWER 1.0 //TODO: find the actual value for this

std::ostream& operator<<(std::ostream& o,Shooter::Goal goal){
	switch(goal){
		#define X(NAME) case Shooter::Goal::NAME: return o<<""#NAME;
		X(FORWARD)
		X(OFF)
		X(REVERSE)
		#undef X
		default:
			assert(0);
	}
}

std::ostream& operator<<(std::ostream& o,Shooter){ return o<<"Shooter()"; }

bool operator==(Shooter::Output_applicator,Shooter::Output_applicator){ return true; }

bool operator==(Shooter a,Shooter b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Shooter a,Shooter b){ return !(a==b); }

Shooter::Output Shooter::Output_applicator::operator()(Robot_outputs const& r)const{
	//assuming talon srx is in percent mode

	double power = r.talon_srx[SHOOTER_LOC].power_level;
	if(power == 0) return Shooter::Output::OFF;
	if(power > 0) return Shooter::Output::FORWARD;
	return Shooter::Output::REVERSE; //assuming less than 0 is reverse
}

Robot_outputs Shooter::Output_applicator::operator()(Robot_outputs r,Shooter::Output out)const{ 
	r.talon_srx[SHOOTER_LOC] = Talon_srx_output::percent([=]{
		switch(out){
			case Shooter::Output::OFF:
				return 0.0;
			case Shooter::Output::REVERSE:
				return -SHOOTER_POWER;
			case Shooter::Output::FORWARD:
				return SHOOTER_POWER;
			default:
				assert(0);
		}
	}());
	return r;
}

std::set<Shooter::Goal> examples(Shooter::Goal*){
	return {Shooter::Goal::REVERSE,Shooter::Goal::OFF,Shooter::Goal::FORWARD};
}

Shooter::Output control(Shooter::Status_detail, Shooter::Goal goal){
	return goal;
}

bool ready(Shooter::Status,Shooter::Goal){
	return true;
}

#ifdef SHOOTER_TEST
#include "formal.h"

int main(){
	Shooter a;
	tester(a);
}

#endif
