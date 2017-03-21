#include "lights.h"
#include <stdlib.h>
#include <cmath>

using namespace std;

#define LOADING_ADDRESS 0
#define CAMERA_ADDRESS 10

Lights::Goal::Goal(Loading_indicator a,bool b):loading_indicator(a),camera_light(b){}
Lights::Goal::Goal():Lights::Goal(Lights::Loading_indicator::GEARS,true){}

ostream& operator<<(ostream& o, Lights::Loading_indicator a){
	#define X(name) if(a==Lights::Loading_indicator::name)return o<<"Lights::Loading_indicator("#name")";
	X(BALLS) X(GEARS)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Lights::Goal a){
	o<<"Lights::Gear(";
	o<<"loading_indicator:"<<a.loading_indicator;
	o<<" camera_light:"<<a.camera_light;
	return o<<")";
}

ostream& operator<<(ostream& o, Lights::Output a){
	o<<"Lights::Output(";
	o<<"loading_indicator:"<<a.loading_indicator;
	o<<" camera_light:"<<a.camera_light;
	return o<<")";
}

ostream& operator<<(ostream& o, Lights){ return o<<"Lights()";}

Lights::Output::Output():loading_indicator(0),camera_light(0){}
Lights::Output::Output(bool li,bool cl):loading_indicator(li),camera_light(cl){}

bool operator==(Lights::Goal a,Lights::Goal b){
	return a.loading_indicator == b.loading_indicator && a.camera_light == b.camera_light;
}
bool operator<(Lights::Goal a,Lights::Goal b){
	if(a.loading_indicator<b.loading_indicator) return 1;
	if(b.loading_indicator<a.loading_indicator) return 0;
	if(!a.camera_light&&b.camera_light) return 1;
	if(a.camera_light&&!b.camera_light) return 0;
	return 0;
}

bool operator<(Lights::Output a,Lights::Output b){
	if(!a.loading_indicator&&b.loading_indicator) return 1;
	if(a.loading_indicator&&!b.loading_indicator) return 0;
	if(!a.camera_light&&b.camera_light) return 1;
	if(a.camera_light&&!b.camera_light) return 0;
	return 0;
}
bool operator==(Lights::Output a, Lights::Output b){ return a.loading_indicator==b.loading_indicator && a.camera_light==b.camera_light; }
bool operator!=(Lights::Output a, Lights::Output b){ return !(a==b); }

bool operator==(Lights::Output_applicator, Lights::Output_applicator){ return 1; }

bool operator==(Lights::Estimator a, Lights::Estimator b){ return a.time==b.time; }
bool operator!=(Lights::Estimator a, Lights::Estimator b){ return !(a==b); }

bool operator==(Lights a, Lights b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Lights a, Lights b){ return !(a==b);}

Lights::Output Lights::Output_applicator::operator()(Robot_outputs r)const{
	Output out;
	out.loading_indicator=[&]{
		switch(r.relay[LOADING_ADDRESS]){
			case Relay_output::_00: 
			case Relay_output::_01: return 0;
			case Relay_output::_10:
			case Relay_output::_11: return 1;
			default: assert(0);
		}
	}();
	if(r.digital_io[CAMERA_ADDRESS]==Digital_out::zero()) out.camera_light=0;
	else if(r.digital_io[CAMERA_ADDRESS]==Digital_out::one()) out.camera_light=1;
	else assert(0);
	return out;
}

Robot_outputs Lights::Output_applicator::operator()(Robot_outputs r, Lights::Output out)const{
	r.relay[LOADING_ADDRESS]=out.loading_indicator?Relay_output::_10:Relay_output::_00;
	r.digital_io[CAMERA_ADDRESS]=out.camera_light?Digital_out::one():Digital_out::zero();
	//r.pwm[9]=.5;
	return r;
}

Lights::Estimator::Estimator():time(0){}

void Lights::Estimator::update(Time now,Input,Output){
	time=now;
}

Lights::Status_detail Lights::Estimator::get()const{
	return time;
}

set<Lights::Goal> examples(Lights::Goal*){ 
	return {
		Lights::Goal{Lights::Loading_indicator::GEARS, 0},
		Lights::Goal{Lights::Loading_indicator::GEARS, 1},
		Lights::Goal{Lights::Loading_indicator::BALLS, 0},
		Lights::Goal{Lights::Loading_indicator::BALLS, 1}
	};
}

set<Lights::Status_detail> examples(Lights::Status_detail*){ return {0, 1}; }

set<Lights::Output> examples(Lights::Output*){
	return {
		Lights::Output{0, 0},
		Lights::Output{0, 1},
		Lights::Output{1, 0},
		Lights::Output{1, 1}
	};
}

Lights::Output control(Lights::Status_detail status, Lights::Goal goal){
	Lights::Output out;
	if(goal.loading_indicator==Lights::Loading_indicator::GEARS) out.loading_indicator=1;
	else out.loading_indicator=(int)floor(status)%2==0;
	out.camera_light=goal.camera_light;
	return out;
}

Lights::Status status(Lights::Status_detail status){ return status; }

bool ready(Lights::Status, Lights::Goal){ return 1;}

#ifdef LIGHTS_TEST
#include "formal.h"

int main(){
	Lights a;
	tester(a);
}

#endif
