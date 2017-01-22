#include "gear_shifter.h"
#include "drivebase.h"

using namespace std;

ostream& operator<<(ostream& o,Gear_shifter const&){
	return o<<"Gear_shifter(...)";
}

#if 0
ostream& operator<<(ostream& o,Gear_shifter::Input a){
	o<<"(";
	o<<a.current<<" "<<a.speed;
	return o<<")";
}

set<Gear_shifter::Input> examples(const Gear_shifter::Input*){
	set<Gear_shifter::Input> r;
	for(double current:{0,1,10}){
		for(double speed:{0/*,1,10*/}){
			r|={current,speed};
		}
	}
	return r;
}

bool operator<(Gear_shifter::Input a,Gear_shifter::Input b){
	if(a.current<b.current) return 1;
	if(b.current<a.current) return 0;
	return a.speed<b.speed;
}

bool operator!=(Gear_shifter::Input a,Gear_shifter::Input b){
	return !(a==b);
}

bool operator==(Gear_shifter::Input a,Gear_shifter::Input b){
	return a.current==b.current && a.speed==b.speed;
}
#endif

set<pair<Digital_in,Digital_in>> examples(const pair<Digital_in,Digital_in>*){
	using T=pair<Digital_in,Digital_in>;
	set<T> r;
	for(auto a:examples((Digital_in*)0)){
		for(auto b:examples((Digital_in*)0)){
			r|=make_pair(a,b);
		}
	}
	return r;
}

set<Drivebase::Encoder_ticks> examples(const Drivebase::Encoder_ticks*){
	set<Drivebase::Encoder_ticks> r;
	for(auto a:{0,1,2}){
		for(auto b:{1,2,3}){
			r|=Drivebase::Encoder_ticks{a,b};
		}
	}
	return r;
}

#if 0
Robot_inputs Gear_shifter::Input_reader::operator()(Robot_inputs robot_inputs,Gear_shifter::Input input)const{
	Drivebase::Input db{
		[=](){
			array<double,Drivebase::MOTORS> r;
			for(auto& a:r){
				a=0;
			}
			r[0]=input.current;
			return r;
		}(),
		example((Drivebase::Encoder_info*)nullptr),
		example((Drivebase::Encoder_info*)nullptr),
		example((Drivebase::Encoder_ticks*)nullptr)
	};
	return Drivebase::Input_reader{}(robot_inputs,db);
}

Gear_shifter::Input Gear_shifter::Input_reader::operator()(Robot_inputs const& robot_inputs)const{
	auto db=Drivebase::Input_reader{}(robot_inputs);
	double speed=0;//TODO: Fixme.
	return {sum(db.current),speed};
}
#endif

ostream& operator<<(ostream& o,Gear_shifter::Output a){
	#define X(A) if(a==Gear_shifter::Output::A) return o<<""#A;
	GEAR_SHIFTER_OUTPUTS(X)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Gear_shifter::Goal a){
	#define X(A) if(a==Gear_shifter::Goal::A) return o<<""#A;
	GEAR_SHIFTER_GOALS(X)
	#undef X
	assert(0);
}

set<Gear_shifter::Output> examples(const Gear_shifter::Output*){
	#define X(A) Gear_shifter::Output::A,
	return {GEAR_SHIFTER_OUTPUTS(X)};
	#undef X
}

static const int SHIFTER_SOLENOID=2;//TOTALLY MADE UP

Robot_outputs Gear_shifter::Output_applicator::operator()(Robot_outputs robot_outputs,Gear_shifter::Output device_output)const{
	//polarity of this is not yet confirmed.
	robot_outputs.solenoid[SHIFTER_SOLENOID]=(device_output==Gear_shifter::Output::HIGH);
	return robot_outputs;
}

Gear_shifter::Output Gear_shifter::Output_applicator::operator()(Robot_outputs robot_outputs)const{
	return robot_outputs.solenoid[SHIFTER_SOLENOID]?Gear_shifter::Output::HIGH:Gear_shifter::Output::LOW;
}

set<bool> examples(const bool*){ return {0,1}; }

set<Gear_shifter::Goal> examples(const Gear_shifter::Goal*){
	#define X(A) Gear_shifter::Goal::A,
	return {GEAR_SHIFTER_GOALS(X)};
	#undef X
}

//set<Gear_shifter::

Gear_shifter::Output control(Gear_shifter::Status_detail status,Gear_shifter::Goal goal){
	switch(goal){
		case Gear_shifter::Goal::HIGH:
			return Gear_shifter::Output::HIGH;
		case Gear_shifter::Goal::LOW:
			return Gear_shifter::Output::LOW;
		case Gear_shifter::Goal::AUTO:
			return status;
		default:
			assert(0);
	}
}

Gear_shifter::Estimator::Speed_tracker::Speed_tracker():
	last_time(0),
	last_ticks(0)
{}

double Gear_shifter::Estimator::Speed_tracker::update(Time now,int ticks){
	auto d_time=now-last_time;
	auto d_ticks=ticks-last_ticks;
	last_time=now;
	last_ticks=ticks;

	if(d_time==0) return 0;//could start by making this do an assert.
	return d_ticks/d_time;
}

Gear_shifter::Estimator::Estimator():
	last_current(0),
	last_output(Output::LOW),//the robot should be built such that disable, etc=LOW GEAR
	recommended(Output::LOW)
{}

double mean(double a,double b){
	return (a+b)/2;
}

void Gear_shifter::Estimator::update(Time now,Gear_shifter::Input input,Gear_shifter::Output output){
	no_shift.update(now,1);//counts down even when robot is disabled.
	if(output!=last_output){
		no_shift.set(2);
	}

	double current_spike_amount=sum(input.current)-last_current;
	last_current=sum(input.current);
	bool current_spike=current_spike_amount>50;//TODO: Test this value.

	auto speed_l=speed_tracker_l.update(now,input.ticks.l);
	auto speed_r=speed_tracker_r.update(now,input.ticks.r);

	//if shifted too recently, then don't shift
	if(!no_shift.done()){
		recommended=output;
		return;
	}

	{
		//don't attempt to shift in the middle of a tight turn.
		static const double TURN_THRESHOLD=1.2;
		if(speed_l>TURN_THRESHOLD*speed_r || speed_r>TURN_THRESHOLD*speed_l){
			recommended=output;
			return;
		}
	}
	double speed=mean(speed_l,speed_r);

	//shift down because going slow?
	if(speed<8){
		recommended=Gear_shifter::Output::LOW;
		return;
	}

	//shift down because of current spike?
	if(current_spike){
		recommended=Gear_shifter::Output::LOW;
		return;
	}

	//shift up because going fast?
	if(speed>10){
		recommended=Gear_shifter::Output::HIGH;
		return;
	}

	//if no reason to change, then go with the most recent thing.
	recommended=output;
}

Gear_shifter::Status_detail Gear_shifter::Estimator::get()const{
	return recommended;
}

Gear_shifter::Status status(Gear_shifter::Status_detail){
	return {};
}

bool ready(Nop::Input,Gear_shifter::Goal){
	return 1;
}

#ifdef GEAR_SHIFTER_TEST
#include "formal.h"
int main(){
	Gear_shifter g;
	tester(g);
}
#endif
