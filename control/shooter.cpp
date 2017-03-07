#include "shooter.h"
#include <cmath>
#include <cassert>

#define SHOOTER_LOC 1


Shooter::Goal::Goal():value(0){}
Shooter::Goal::Goal(double p):value(p){}

std::ostream& operator<<(std::ostream& o,Shooter::Goal goal){
	o<<"Shooter::Goal(";
	o<<" value:"<<goal.value;
	return o<<")";
}

std::ostream& operator<<(std::ostream& o,Shooter){ return o<<"Shooter()"; }

bool operator==(Shooter::Goal a,Shooter::Goal b){
	 return a.value==b.value;
}

bool operator!=(Shooter::Goal a,Shooter::Goal b){ return !(a==b); }
bool operator<(Shooter::Goal a,Shooter::Goal b){
	#define X(NAME) if(a.NAME<b.NAME) return 1; if(b.NAME<a.NAME) return 0;
	X(value)
	#undef X
	return 0;
}

bool operator==(Shooter::Output_applicator,Shooter::Output_applicator){ return true; }

bool operator==(Shooter a,Shooter b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Shooter a,Shooter b){ return !(a==b); }

Shooter::Output Shooter::Output_applicator::operator()(Robot_outputs const& r)const{
	return r.talon_srx[SHOOTER_LOC].power_level;
}

Robot_outputs Shooter::Output_applicator::operator()(Robot_outputs r,Shooter::Output out)const{ 
	r.talon_srx[SHOOTER_LOC].mode = Talon_srx_output::Mode::VOLTAGE;
	r.talon_srx[SHOOTER_LOC].power_level=out.value;
	return r;
}

std::set<Shooter::Goal> examples(Shooter::Goal*){
	return {{0},{1}};
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
