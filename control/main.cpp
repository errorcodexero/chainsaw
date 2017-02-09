#include "main.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <math.h>
#include <stdlib.h>
#include "toplevel.h"
#include "../util/util.h"
#include "../input/util.h"
#include <vector>
#include "../executive/teleop.h"

using namespace std;

static int print_count=0;

//TODO: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():
	mode(Executive{Teleop()}),
	autonomous_start(0)
{}

template<size_t LEN>
array<double,LEN> floats_to_doubles(array<float,LEN> a){
	array<double,LEN> r;
	for(size_t i=0;i<LEN;i++) r[i]=a[i];
	return r;
}

Robot_outputs Main::operator()(Robot_inputs in,ostream&){
	print_count++;

	static const unsigned MAIN_JOYSTICK_PORT = 0, GUNNER_JOYSTICK_PORT = 1, PANEL_PORT = 2;

	perf.update(in.now);
	
	Joystick_data main_joystick=in.joystick[MAIN_JOYSTICK_PORT];
	Joystick_data gunner_joystick=in.joystick[GUNNER_JOYSTICK_PORT];//TODO: remove
	Panel panel=interpret_oi(in.joystick[PANEL_PORT]);
	if(!panel.in_use){
		panel = interpret_gamepad(in.joystick[GUNNER_JOYSTICK_PORT]);
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
	bool autonomous_start_now=autonomous_start(in.robot_mode.autonomous && in.robot_mode.enabled);
	Toplevel::Goal goals = mode.run(Run_info{in,main_joystick,gunner_joystick,panel,toplevel_status});
	
	auto next=mode.next_mode(Next_mode_info{in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),panel,in});
	since_switch.update(in.now,mode!=next);
	mode=next;
		
	Toplevel::Output r_out=control(toplevel_status,goals); 
	auto r=toplevel.output_applicator(Robot_outputs{},r_out);
	
	r=force(r);
	auto input=toplevel.input_reader(in);

	toplevel.estimator.update(
		in.now,
		input,
		toplevel.output_applicator(r)
	);
	log(in,toplevel_status,r);

	if(in.ds_info.connected && (print_count % 10 == 0)){
		cout<<"mode: "<<mode<<"\n";
		cout<<"panel:"<<panel<<"\n";
	}

	return r;
}

bool operator==(Main const& a,Main const& b){
	return a.force==b.force && 
		a.perf==b.perf && 
		a.toplevel==b.toplevel && 
		a.since_switch==b.since_switch && 
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
	o<<" "<<m.mode;
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
}

#endif
