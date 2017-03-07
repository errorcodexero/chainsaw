#include "shooter.h"
#include <cmath>
#include <cassert>
#include <iostream>

#define SHOOTER_LOC 1

using namespace std;

Shooter::Estimator::Estimator():last({}){}

Shooter::Goal::Goal():value(0){}
Shooter::Goal::Goal(double p):value(p){}

ostream& operator<<(ostream& o,Shooter::Goal goal){
	o<<"Shooter::Goal(";
	o<<" value:"<<goal.value;
	return o<<")";
}

ostream& operator<<(ostream& o,Shooter::Estimator a){ return o<<"Shooter::Estimator( last:"<<a.get()<<" last_output:"<<a.last_output<<")"; }
ostream& operator<<(ostream& o,Shooter::Input a){ return o<<"Shooter::Input(enabled:"<<a.enabled<<")"; }
ostream& operator<<(ostream& o,Shooter a){ return o<<"Shooter("<<a.estimator<<")"; }

bool operator==(Shooter::Input a,Shooter::Input b){ return a.enabled==b.enabled; }
bool operator!=(Shooter::Input a,Shooter::Input b){ return !(a==b); }
bool operator<(Shooter::Input a,Shooter::Input b){
	return !a.enabled && b.enabled;
} 

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

bool operator<(Shooter::Input_reader,Shooter::Input_reader){ return false; }
bool operator==(Shooter::Input_reader,Shooter::Input_reader){ return true; }

bool operator==(Shooter::Estimator a,Shooter::Estimator b){ return a.last==b.last && a.last_output==b.last_output; }
bool operator!=(Shooter::Estimator a,Shooter::Estimator b){ return !(a==b); }

bool operator==(Shooter::Output_applicator,Shooter::Output_applicator){ return true; }

bool operator==(Shooter a,Shooter b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Shooter a,Shooter b){ return !(a==b); }

Shooter::Input Shooter::Input_reader::operator()(Robot_inputs const& r)const{
	return {r.robot_mode.enabled};
}

Robot_inputs Shooter::Input_reader::operator()(Robot_inputs r,Shooter::Input in)const{
	r.robot_mode.enabled=in.enabled;
	return r;
}

Shooter::Output Shooter::Output_applicator::operator()(Robot_outputs const& r)const{
	return r.talon_srx[SHOOTER_LOC].power_level;
}

Robot_outputs Shooter::Output_applicator::operator()(Robot_outputs r,Shooter::Output out)const{ 
	r.talon_srx[SHOOTER_LOC].mode = Talon_srx_output::Mode::VOLTAGE;
	r.talon_srx[SHOOTER_LOC].power_level=out.value;
	return r;
}

Shooter::Status_detail Shooter::Estimator::get()const{
	return last;
}

void Shooter::Estimator::update(Time,Shooter::Input,Shooter::Output output,ostream&){
	//TODO
	last_output=output;
} 

set<Shooter::Input> examples(Shooter::Input*){
	return {
		{true},
		{false}
	}; 
}
set<Shooter::Goal> examples(Shooter::Goal*){
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
