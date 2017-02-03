#include "drivebase.h"
#include "../util/fixVictor.h"
#include <iostream>
#include <math.h>
#include "../util/util.h"

using namespace std;

#define L_MOTOR_LOC_1 0
#define L_MOTOR_LOC_2 1
#define L_MOTOR_LOC_3 2
#define R_MOTOR_LOC_1 3
#define R_MOTOR_LOC_2 4
#define R_MOTOR_LOC_3 5

unsigned pdb_location(Drivebase::Motor m){
	#define X(NAME,INDEX) if(m==Drivebase::NAME) return INDEX;
	//WILL NEED CORRECT VALUES
	X(LEFT1,1)
	X(LEFT2,2)
	X(LEFT3,3)
	X(RIGHT1,13)
	X(RIGHT2,14)
	X(RIGHT3,15)
	#undef X
	assert(0);
	//assert(m>=0 && m<Drivebase::MOTORS);
}

int encoderconv(Maybe_inline<Encoder_output> encoder){
	if(encoder) return *encoder;
	return 10000;
}

double ticks_to_inches(const int ticks){
	const unsigned int TICKS_PER_REVOLUTION=200;//for 2017
	const double WHEEL_DIAMETER=4.25;//inches for 2017
	const double WHEEL_CIRCUMFERENCE=WHEEL_DIAMETER*PI;//inches
	const double INCHES_PER_TICK=WHEEL_CIRCUMFERENCE/(double)TICKS_PER_REVOLUTION;
	const double ERROR_CORRECTION = 0.181952663;//2017, encoders are geared. Rough calculation.
	return ticks*INCHES_PER_TICK*ERROR_CORRECTION;
}

#define R_ENCODER_PORTS 0,1
#define L_ENCODER_PORTS 2,3
#define L_ENCODER_LOC 0
#define R_ENCODER_LOC 1

Robot_inputs Drivebase::Input_reader::operator()(Robot_inputs all,Input in)const{
	for(unsigned i=0;i<MOTORS;i++){
		all.current[pdb_location((Motor)i)]=in.current[i];
	}
	auto set=[&](unsigned index,Digital_in value){
		all.digital_io.in[index]=value;
	};
	auto encoder=[&](unsigned a,unsigned b,Encoder_info e){
		set(a,e.first);
		set(b,e.second);
	};
	encoder(L_ENCODER_PORTS,in.left);
	encoder(R_ENCODER_PORTS,in.right);
	all.digital_io.encoder[L_ENCODER_LOC] = in.ticks.l;
	all.digital_io.encoder[R_ENCODER_LOC] = in.ticks.r;
	return all;
}

Drivebase::Input Drivebase::Input_reader::operator()(Robot_inputs const& in)const{
	auto encoder_info=[&](unsigned a, unsigned b){
		return make_pair(in.digital_io.in[a],in.digital_io.in[b]);
	};
	return Drivebase::Input{
		[&](){
			array<double,Drivebase::MOTORS> r;
			for(unsigned i=0;i<Drivebase::MOTORS;i++){
				Drivebase::Motor m=(Drivebase::Motor)i;
				r[i]=in.current[pdb_location(m)];
			}
			return r;
		}(),
		encoder_info(L_ENCODER_PORTS),
		encoder_info(R_ENCODER_PORTS),
		{encoderconv(in.digital_io.encoder[L_ENCODER_LOC]),encoderconv(in.digital_io.encoder[R_ENCODER_LOC])}
	};
}

float range(const Robot_inputs in){
	float volts=in.analog[2];
	const float voltsperinch=1; 
	float inches=volts*voltsperinch;
	return inches;
}

IMPL_STRUCT(Drivebase::Encoder_ticks::Encoder_ticks,ENCODER_TICKS)
IMPL_STRUCT(Drivebase::Speeds::Speeds,SPEEDS_ITEMS)

IMPL_STRUCT(Drivebase::Status::Status,DRIVEBASE_STATUS)
IMPL_STRUCT(Drivebase::Input::Input,DRIVEBASE_INPUT)
IMPL_STRUCT(Drivebase::Output::Output,DRIVEBASE_OUTPUT)

CMP_OPS(Drivebase::Encoder_ticks,ENCODER_TICKS)
CMP_OPS(Drivebase::Speeds,SPEEDS_ITEMS)

CMP_OPS(Drivebase::Input,DRIVEBASE_INPUT)

CMP_OPS(Drivebase::Status,DRIVEBASE_STATUS)

set<Drivebase::Status> examples(Drivebase::Status*){
	return {Drivebase::Status{
		array<Motor_check::Status,Drivebase::MOTORS>{
			Motor_check::Status::OK_,
			Motor_check::Status::OK_
		}
		,
		false,
		{0.0,0.0},
		{0,0}
	}};
}

set<Drivebase::Goal> examples(Drivebase::Goal*){
	return {
		Drivebase::Goal{0,0},
		Drivebase::Goal{1,1}
	};
}

ostream& operator<<(ostream& o,Drivebase::Goal const& a){
	return o<<"Drivebase::Goal("<<a.left<<" "<<a.right<<")";
}

#define CMP(name) if(a.name<b.name) return 1; if(b.name<a.name) return 0;

bool operator<(Drivebase::Goal const& a,Drivebase::Goal const& b){
	CMP(left)
	CMP(right)
	return 0;
}

CMP_OPS(Drivebase::Output,DRIVEBASE_OUTPUT)

set<Drivebase::Output> examples(Drivebase::Output*){
	return {
		Drivebase::Output{0,0},
		Drivebase::Output{1,1}
	};
}

set<Drivebase::Input> examples(Drivebase::Input*){
	auto d=Digital_in::_0;
	auto p=make_pair(d,d);
	return {Drivebase::Input{
		{0,0,0,0,0,0},p,p,{0,0}
	}};
}

Drivebase::Estimator::Estimator():motor_check(),last({{{}},false,{0,0},{0,0}}){
	timer.set(.05);
}

Drivebase::Status_detail Drivebase::Estimator::get()const{
	/*array<Motor_check::Status,MOTORS> a;
	for(unsigned i=0;i<a.size();i++){
		a[i]=motor_check[i].get();
	}*/
	
	return last;//Status{a,stall,piston/*,speeds,last_ticks*/};
}

ostream& operator<<(ostream& o,Drivebase::Output_applicator){
	return o<<"output_applicator";
}

ostream& operator<<(ostream& o,Drivebase const& a){
	return o<<"Drivebase("<<a.estimator.get()<<")";
}

double get_output(Drivebase::Output out,Drivebase::Motor m){
	#define X(NAME,POSITION) if(m==Drivebase::NAME) return out.POSITION;
	X(LEFT1,l)
	X(LEFT2,l)
	X(LEFT3,l)
	X(RIGHT1,r)
	X(RIGHT2,r)
	X(RIGHT3,r)
	#undef X
	assert(0);
}
double sum(std::array<double, 6ul> a){
	double sum = 0;
	for(unsigned int i=0;i<a.size();i++)
		sum+=a[i];

	return sum;
}
double mean(std::array<double, 6ul> a){
	return sum(a)/a.size();
}
void Drivebase::Estimator::update(Time now,Drivebase::Input in,Drivebase::Output out){\
	timer.update(now,true);
	static const double POLL_TIME = .05;//seconds
	if(timer.done()){
		last.ticks = in.ticks;
		last.ticks.l = -last.ticks.l;//because encoder is reversed
		last.speeds.l = ticks_to_inches((last.ticks.l-in.ticks.l)/POLL_TIME);
		last.speeds.r = ticks_to_inches((last.ticks.r-in.ticks.r)/POLL_TIME);
		timer.set(POLL_TIME);
	}
	
	//cout << "Encoder in: " << in << endl;
	for(unsigned i=0;i<MOTORS;i++){
		Drivebase::Motor m=(Drivebase::Motor)i;
		auto current=in.current[i];
		auto set_power_level=get_output(out,m);
		motor_check[i].update(now,current,set_power_level);
		
	}
	last.stall = mean(in.current) > 5;
}

Robot_outputs Drivebase::Output_applicator::operator()(Robot_outputs robot,Drivebase::Output b)const{
	//cout<<"\nOutputs: "<<b<<"\n";
	robot.pwm[L_MOTOR_LOC_1] = -b.l;
	robot.pwm[L_MOTOR_LOC_2] = -b.l;
	robot.pwm[L_MOTOR_LOC_3] = -b.l;
	robot.pwm[R_MOTOR_LOC_1] = -b.r;
	robot.pwm[R_MOTOR_LOC_2] = -b.r;
	robot.pwm[R_MOTOR_LOC_3] = -b.r;

	/*
	cout<<"b.l: "<<b.l<<'\n';
	cout<<"b.r: "<<b.r<<'\n';
	cout<<"pwms: "<<robot.pwm<<'\n';
	*/

	auto set_encoder=[&](unsigned int a, unsigned int b,unsigned int loc){
		robot.digital_io[a] = Digital_out::encoder(loc,1);
		robot.digital_io[b] = Digital_out::encoder(loc,0);
	};
	
	set_encoder(L_ENCODER_PORTS,L_ENCODER_LOC);
	set_encoder(R_ENCODER_PORTS,R_ENCODER_LOC);
	/*robot.digital_io[0]=Digital_out::encoder(0,1);
	robot.digital_io[1]=Digital_out::encoder(0,0);
	robot.digital_io[2]=Digital_out::encoder(1,1);
	robot.digital_io[3]=Digital_out::encoder(1,0);
	robot.digital_io[4]=Digital_out::encoder(2,1);
	robot.digital_io[5]=Digital_out::encoder(2,0);*/
	return robot;
}

Drivebase::Output Drivebase::Output_applicator::operator()(Robot_outputs robot)const{
	//assuming both motors on the same side are set to the same value//FIXME ?
	return Drivebase::Output{	
		-robot.pwm[L_MOTOR_LOC_1],
		-robot.pwm[R_MOTOR_LOC_1],
	};
}

bool operator==(Drivebase::Output_applicator const&,Drivebase::Output_applicator const&){
	return true;
}

bool operator==(Drivebase::Estimator const& a,Drivebase::Estimator const& b){
	if(a.last != b.last) return false;
	if(a.timer != b.timer) return false;
	/*for(unsigned i=0; i<Drivebase::MOTORS; i++){
		if(a.motor_check[i]!=b.motor_check[i])return false;
	}*/
	return true;
}

bool operator!=(Drivebase::Estimator const& a,Drivebase::Estimator const& b){
	return !(a==b);
}

bool operator==(Drivebase const& a,Drivebase const& b){
	return a.estimator==b.estimator && a.output_applicator==b.output_applicator;
}

bool operator!=(Drivebase const& a,Drivebase const& b){
	return !(a==b);
}

Drivebase::Output control(Drivebase::Status /*status*/,Drivebase::Goal goal){
	return Drivebase::Output{goal.left,goal.right};
}

Drivebase::Status status(Drivebase::Status a){ return a; }

bool ready(Drivebase::Status,Drivebase::Goal){ return 1; }

#ifdef DRIVEBASE_TEST
#include "formal.h"
int main(){
	Drivebase d;
	tester(d);
}
#endif
