#include "main.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <math.h>
#include <stdlib.h>
#include "toplevel.h"
#include "../util/util.h"
#include "../input/util.h"
#include "../util/point.h"
#include <vector>
#include <assert.h>
#include <fstream>
#include "../executive/teleop.h"

using namespace std;

ofstream myfile2;

static int print_count=0;
#define SLOW_PRINT (print_count%10==0)

//TODO: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():
	//mode(Mode::TELEOP),
	mode(Executive{Teleop()}),
	/*motion_profile(0.0,.01),
	in_br_range(),*/
	autonomous_start(0)
{
	/*in_br_range.set(2.0);
	set_initial_encoders = true;
	initial_encoders = make_pair(0,0);
	br_step=0;
	myfile2.open(NAVLOG2);
	myfile2 << "test start" << endl;*/
}


template<size_t LEN>
array<double,LEN> floats_to_doubles(array<float,LEN> a){
	array<double,LEN> r;
	for(size_t i=0;i<LEN;i++) r[i]=a[i];
	return r;
}

pair<float,float> driveatwall(const Robot_inputs in){
	const float targetinches=3; //Desired distance from wall
	float currentinches=range(in);
	pair<float,float> motorvoltmods;
	//motorvoltmods.first = left; motorvoltmods.second = right
	float adjustment=0;
	if(targetinches<currentinches){
		motorvoltmods.first=adjustment;
		motorvoltmods.second=adjustment*-1;
	}
	return motorvoltmods;
}

/*double ticks_to_inches(int ticks){//Moved to drivebase.cpp
	const unsigned int TICKS_PER_REVOLUTION=100;
	const double WHEEL_DIAMETER=8.0;//inches
	const double WHEEL_CIRCUMFERENCE=WHEEL_DIAMETER*PI;//inches
	const double INCHES_PER_TICK=WHEEL_CIRCUMFERENCE/(double)TICKS_PER_REVOLUTION;//0.25 vs 0.251327
	return ticks*INCHES_PER_TICK;
}*/

double ticks_to_degrees(int ticks){
	const double DEGREES_PER_TICK=0.716197244;//Assumed for now
	return ticks * DEGREES_PER_TICK;//Degrees clockwise
}

Robot_outputs Main::operator()(Robot_inputs in,ostream&){
	print_count++;

	perf.update(in.now);
	Joystick_data main_joystick=in.joystick[0];
	Joystick_data gunner_joystick=in.joystick[1];
	Panel panel=interpret(in.joystick[2]);

	if(!in.robot_mode.enabled){
	}

	force.update(
		main_joystick.button[Gamepad_button::A],
		main_joystick.button[Gamepad_button::LB],
		main_joystick.button[Gamepad_button::RB],
		main_joystick.button[Gamepad_button::BACK],
		main_joystick.button[Gamepad_button::B],
		main_joystick.button[Gamepad_button::X]
	);
	
	Toplevel::Status_detail toplevel_status=toplevel.estimator.get();
		
	if(in.ds_connected && SLOW_PRINT) cout<<"panel:"<<panel<<"\n";
	//cout << "Goals: " << motion_profile.goal << " Current: " << ticks_to_inches(toplevel_status.drive.ticks.first/*in.digital_io.encoder[0]*/) << endl;
	
	//if(SLOW_PRINT) cout<<"br_step:"<<br_step<<"\n";
	
	bool autonomous_start_now=autonomous_start(in.robot_mode.autonomous && in.robot_mode.enabled);
	since_auto_start.update(in.now,autonomous_start_now);
		
	Toplevel::Goal goals;
	//decltype(in.current) robotcurrent;
	//for(auto &a:robotcurrent) a = 0;
	/*if((toplevel_status.drive.ticks.first && initial_encoders.first==10000) || (toplevel_status.drive.ticks.second && initial_encoders.second==10000)) set_initial_encoders=true;
	if(set_initial_encoders){
		set_initial_encoders=false;
		cout<<"\nSET INITIAL ENCODER VALUES\n";
		initial_encoders = toplevel_status.drive.ticks;	
	}*/
	goals = mode.run(Run_info{in,main_joystick,gunner_joystick,panel,toplevel_status});
	
	auto next=mode.next_mode(Next_mode_info{in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),panel,in});
	if(in.ds_connected && SLOW_PRINT) cout<<"mode: "<<mode<<"\n";
	//next_mode(mode,in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),panel,topready,in,initial_encoders,br_step,set_initial_encoders,motion_profile,in_br_range);
	
	since_switch.update(in.now,mode!=next);
	mode=next;
		
	Toplevel::Output r_out=control(toplevel_status,goals); 
	auto r=toplevel.output_applicator(Robot_outputs{},r_out);
	
	r=force(r);
	auto input=toplevel.input_reader(in);

	/*auto talonPower = Talon_srx_output();
	talonPower.power_level = .5;
	r.talon_srx[0]= talonPower;*/
	
	toplevel.estimator.update(
		in.now,
		input,
		toplevel.output_applicator(r)
	);
	log(in,toplevel_status,r);
	return r;
}

bool operator==(Main const& a,Main const& b){
	return a.force==b.force && 
		a.perf==b.perf && 
		a.toplevel==b.toplevel && 
		a.since_switch==b.since_switch && 
		a.since_auto_start==b.since_auto_start &&
		a.autonomous_start==b.autonomous_start;
}

bool operator!=(Main const& a,Main const& b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Main const& m){
	o<<"Main(";
	o<<" "<<m.force;
	o<<" "<<m.perf;
	o<<" "<<m.toplevel;
	o<<" "<<m.since_switch;
	return o<<")";
}

bool approx_equal(Main a,Main b){
	if(a.force!=b.force) return 0;
	if(a.toplevel!=b.toplevel) return 0;
	return 1;
}

#ifdef MAIN_TEST
#include<fstream>
#include "monitor.h"

template<typename T>
vector<T> uniq(vector<T> v){
	vector<T> r;
	for(auto a:v){
		if(!(r.size()&&r[r.size()-1]==a)){
			r|=a;
		}
	}
	return r;
}

int main(){
	//test_next_mode();
	//test_modes();
}

#endif
