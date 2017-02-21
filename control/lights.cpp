#include "lights.h"
#include <stdlib.h>

using namespace std;

#define LIGHTS_ADDRESS 0

ostream& operator<<(ostream& o, Loading_indicator a){
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

ostream& operator<<(ostream& o, Lights){ return o<<"Lights()";}

bool operator==(Lights::Output_applicator, Lights::Output_applicator){return 1;}

bool operator==(Lights a, Lights b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Lights a, Lights b){ return !(a==b);}

Lights::Output Lights::Output_applicator::operator()(Robot_outputs r)const{
	//Assuming that the indicator light is always set to indicate balls if that channel is 0 and gears if it's 1
	switch(r.relay[LIGHTS_ADDRESS]){
		case Relay_output::_00: return Output{Lights::Indicator_light::BALLS, 0};
		case Relay_output::_01: return Output{Lights::Indicator_light::BALLS, 1};
		case Relay_output::_10: return Output{Lights::Indicator_light::GEARS, 0};
		case Relay_output::_11: return Output{Lights::Indicator_light::GEARS, 1};
		default: assert(0);
	}
}

Robot_outputs Lights::Output_applicator::operator()(Robot_outputs r, Lights::Output out)const{
	if(!out.loading_indicator&&!out.camera_light) r.relay[LIGHTS_ADDRESS]=Relay_output::_00;
	else if(!out.loading_indicator&&out.camera_light) r.relay[LIGHTS_ADDRESS]=Relay_output::_01;
	else if(out.loading_indicator&&!out.camera_light) r.relay[LIGHTS_ADDRESS]=Relay_output::_10;
	else if(out.loading_indicator&&out.camera_light) r.relay[LIGHTS_ADDRESS]=Relay_output::_11;
	else assert(0);
	return r;
}

Lights::Estimator::Estimator():time(0){}

void Lights::Estimator::update(Time now,Input,Output){
	time=now;
}

Status_detail Lights::Estimator::get(){
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

Lights::Output control(Lights::Status_detail status, Lights::Goal goal){
	Lights::Output out;
	if(goal.loading_indicator==Lights::Loading_indicator::GEARS) out.loading_indicator=1;
	else out.loading_indicator=floor(status)%2==0;
	out.camera_light=goal.camera_light;
	return out;
}

bool ready(Lights::Status, Lights::Goal){ return 1;}

#ifdef LIGHTS_TEST
#include "formal.h"

int main(){
	Lights a;
	tester(a);
}

#endif
