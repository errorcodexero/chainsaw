#include "step.h"
#include<queue>
#include "executive.h"
#include "../util/util.h"
#include <cmath>
//#include "teleop.h"

using namespace std;

//this part should eventually go into util
template<typename T>
queue<T>& operator|=(queue<T> &q,T t){
	q.push(t);
	return q;
}

ostream& operator<<(ostream& o,Step const& a){
	a.display(o);
	return o;
}

//This part stays in the CPP file.
Toplevel::Goal Step::run(Run_info info, Toplevel::Goal goals){
	return impl->run(info,goals);
}
Toplevel::Goal Step::run(Run_info info){
	return impl->run(info,{});
}

const double RIGHT_SPEED_CORRECTION = /*-0.045; */ 0.0;// 0 is for comp bot. //left and right sides of the practice robot drive at different speeds given the same power, adjust this to make the robot drive straight

Drivebase::Distances Turn::angle_to_distances(Rad target_angle){
	Inch side_goal = target_angle * 0.5 * ROBOT_WIDTH;
	return Drivebase::Distances{side_goal,-side_goal};
}

Drivebase::Distances Turn::get_distance_travelled(Drivebase::Distances current){
	return current - initial_distances;
}

Turn::Turn(Rad a):Turn(a,0.02,0.5){}//from testing
Turn::Turn(Rad a,double vel_modifier,double max):target_angle(a),initial_distances({0,0}),init(false),side_goals(angle_to_distances(a)){
	motion_profile = {side_goals.l,vel_modifier,max};//from testing
}

Toplevel::Goal Turn::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Turn::run(Run_info info,Toplevel::Goal goals){
	if(!init){
		initial_distances = info.status.drive.distances;
		init = true;
	}
	Drivebase::Distances distance_travelled = get_distance_travelled(info.status.drive.distances);
	
	//ignoring right encoder because it's proven hard to get meaningful data from it
	double power = motion_profile.target_speed(distance_travelled.l); 
	goals.drive.left = clip(target_to_out_power(power));//TODO: move .2 to the constructor of Turn and set an instance variable
	goals.drive.right = -clip(target_to_out_power(power - RIGHT_SPEED_CORRECTION * power));
	goals.shifter = Gear_shifter::Goal::LOW;
	return goals;
}

Step::Status Turn::done(Next_mode_info info){
	static const Inch TOLERANCE = 1.0;//inches
	Drivebase::Distances distance_travelled = get_distance_travelled(info.status.drive.distances);
	Drivebase::Distances distance_left = side_goals - distance_travelled;
	if(fabs(distance_left.l) < TOLERANCE){
		 in_range.update(info.in.now,info.in.robot_mode.enabled);
	} else {
		static const Time FINISH_TIME = .50;//seconds
		in_range.set(FINISH_TIME);
	}
	return in_range.done() ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
}

std::unique_ptr<Step_impl> Turn::clone()const{
	return unique_ptr<Step_impl>(new Turn(*this));
}

bool Turn::operator==(Turn const& b)const{
	return target_angle == b.target_angle && initial_distances == b.initial_distances && side_goals == b.side_goals && motion_profile == b.motion_profile && in_range == b.in_range;
}

//Step::Step(Step_impl const& a):impl(a.clone().get()){}
Step::Step(Step_impl const& a){
	auto c=a.clone();
	if(!c)nyi
	impl=move(c);
}

Step::Status Step::done(Next_mode_info a){
	return impl->done(a);
}

void Step::display(ostream& o)const{
	impl->display(o);
}

bool Step::operator==(Step const& a)const{
	assert(impl);
	return impl->operator==(a);
}

bool Step::operator<(Step const& a)const{
	assert(impl);
	return impl->operator<(a);
}

void Step_impl::display(ostream& o)const{
	o<<"Step_impl";
}

Step_impl::~Step_impl(){}

/*bool Step_impl::operator==(Step_impl const& a)const{
	T const& b=dynamic_cast<T const&>(a.get());
	return this->operator==(b);
}*/

Drive_straight::Drive_straight(Inch goal):Drive_straight(goal,0.02,0.5){}
Drive_straight::Drive_straight(Inch goal,double vel_modifier,double max):target_dist(goal),initial_distances(Drivebase::Distances{0,0}),init(false),motion_profile(goal,vel_modifier,max),gear(Gear_shifter::Goal::LOW){}//Motion profiling values from testing

Drivebase::Distances Drive_straight::get_distance_travelled(Drivebase::Distances current){
	return current - initial_distances;
}

Step::Status Drive_straight::done(Next_mode_info info){
	static const Inch TOLERANCE = 3.0;//inches
	Drivebase::Distances distance_travelled = get_distance_travelled(info.status.drive.distances);
	Drivebase::Distances distance_left = Drivebase::Distances{target_dist,target_dist} - distance_travelled;
	//ignoring right encoder because it's proven hard to get meaningful data from it
	if(fabs(distance_left.l) < TOLERANCE){
		in_range.update(info.in.now,info.in.robot_mode.enabled);
	} else {
		static const Time FINISH_TIME = .50;
		in_range.set(FINISH_TIME);
	}
	
	/*
	if(info.status.drive.stall){
		stall_timer.update(info.in.now,info.in.robot_mode.enabled);
	} else{
		static const Time STALL_TIME = 1.0;
		stall_timer.set(STALL_TIME);
	}
	if(stall_timer.done()) return Step::Status::FINISHED_FAILURE;
	*/
	
	//cout<<"stall:"<<info.status.drive.stall<<"\n";

	return in_range.done() ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
}

Toplevel::Goal Drive_straight::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Drive_straight::run(Run_info info,Toplevel::Goal goals){
	if(!init){
		initial_distances = info.status.drive.distances;
		init = true;
	}
	Drivebase::Distances distance_travelled = get_distance_travelled(info.status.drive.distances);

	double power = motion_profile.target_speed(distance_travelled.l); //ignoring right encoder because it's proven hard to get meaningful data from it

	goals.drive.left = clip(target_to_out_power(power));//TODO: move .11 to the constructor of Drive_straight and set an instance variable
	goals.drive.right = clip(target_to_out_power(power + power * RIGHT_SPEED_CORRECTION)); //right side would go faster than the left without error correction
	goals.shifter = gear;
	return goals;
}

unique_ptr<Step_impl> Drive_straight::clone()const{
	return unique_ptr<Step_impl>(new Drive_straight(*this));
}

bool Drive_straight::operator==(Drive_straight const& b)const{
	return target_dist == b.target_dist && initial_distances == b.initial_distances && init == b.init && motion_profile == b.motion_profile && in_range == b.in_range /*&& stall_timer == b.stall_timer*/ && gear == b.gear;
}

Ram::Ram(Inch goal):target_dist(goal),initial_distances(Drivebase::Distances{0,0}),init(false),gear(Gear_shifter::Goal::LOW){}

Drivebase::Distances Ram::get_distance_travelled(Drivebase::Distances current){
	return current - initial_distances;
}

Step::Status Ram::done(Next_mode_info info){
	static const Inch TOLERANCE = 0.0;//inches
	Drivebase::Distances distance_travelled = get_distance_travelled(info.status.drive.distances);
	//Drivebase::Distances distance_left = Drivebase::Distances{target_dist,target_dist} - distance_travelled;
	//ignoring right encoder because it's proven hard to get meaningful data from it
	return (fabs(distance_travelled.l) >= fabs(target_dist) - TOLERANCE) ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
}

Toplevel::Goal Ram::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Ram::run(Run_info info,Toplevel::Goal goals){
	if(!init){
		initial_distances = info.status.drive.distances;
		init = true;
	}

	static const double POWER = .5;
	double p = copysign(POWER,target_dist);

	goals.drive.left = p;
	goals.drive.right = p + p * RIGHT_SPEED_CORRECTION; //right side would go faster than the left without error correction
	goals.shifter = gear;
	return goals;
}

unique_ptr<Step_impl> Ram::clone()const{
	return unique_ptr<Step_impl>(new Ram(*this));
}

bool Ram::operator==(Ram const& b)const{
	return target_dist == b.target_dist && initial_distances == b.initial_distances && init == b.init /*&& stall_timer == b.stall_timer*/ && gear == b.gear;
}

Wait::Wait(Time wait_time){
	wait_timer.set(wait_time);
}

Step::Status Wait::done(Next_mode_info){
	return wait_timer.done() ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
}

Toplevel::Goal Wait::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Wait::run(Run_info info,Toplevel::Goal goals){
	wait_timer.update(info.in.now,info.in.robot_mode.enabled);
	return goals;
}

unique_ptr<Step_impl> Wait::clone()const{
	return unique_ptr<Step_impl>(new Wait(*this));
}

bool Wait::operator==(Wait const& b)const{
	return wait_timer == b.wait_timer;
}

Lift_gear::Lift_gear():gear_goal({Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::UP,Roller::Goal::OFF,Roller_arm::Goal::STOW}){}

Step::Status Lift_gear::done(Next_mode_info info){
	return ready(status(info.status.gear_collector),gear_goal) ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
}

Toplevel::Goal Lift_gear::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Lift_gear::run(Run_info info, Toplevel::Goal goals){
	(void)info;
	goals.gear_collector = gear_goal;
	return goals;
}

unique_ptr<Step_impl> Lift_gear::clone()const{
	return unique_ptr<Step_impl>(new Lift_gear(*this));
}

bool Lift_gear::operator==(Lift_gear const& b)const{
	(void)b;
	return true;
}

Drop_gear::Drop_gear():gear_goal({Gear_grabber::Goal::OPEN,Gear_lifter::Goal::UP,Roller::Goal::OFF,Roller_arm::Goal::STOW}){}

Step::Status Drop_gear::done(Next_mode_info info){	
	return ready(status(info.status.gear_collector),gear_goal) ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
}

Toplevel::Goal Drop_gear::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Drop_gear::run(Run_info info,Toplevel::Goal goals){
	(void)info;
	goals.gear_collector = gear_goal;
	return goals;
}

unique_ptr<Step_impl> Drop_gear::clone()const{
	return unique_ptr<Step_impl>(new Drop_gear(*this));
}

bool Drop_gear::operator==(Drop_gear const& b)const{
	(void)b;
	return true;
}

Drop_collector::Drop_collector():gear_goal({Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::DOWN,Roller::Goal::OFF,Roller_arm::Goal::STOW}){}

Step::Status Drop_collector::done(Next_mode_info info){
	return ready(status(info.status.gear_collector),gear_goal) ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
}

Toplevel::Goal Drop_collector::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Drop_collector::run(Run_info info,Toplevel::Goal goals){
	(void)info;
	goals.gear_collector = gear_goal;
	return goals;
}

unique_ptr<Step_impl> Drop_collector::clone()const{
	return unique_ptr<Step_impl>(new Drop_collector(*this));
}

bool Drop_collector::operator==(Drop_collector const& b)const{
	(void)b;
	return true;
}

void Combo::display(std::ostream& o)const{
	Step_impl_inner<Combo>::display(o);
	o<<"(";
	step_a.display(o);
	o<<" ";
	step_b.display(o);
	o<<")";
}

Combo::Combo(Step a,Step b):step_a(a),step_b(b){}

Step::Status Combo::done(Next_mode_info info){
	Step::Status a_status = step_a.done(info);
	Step::Status b_status = step_b.done(info);
	switch(a_status){
		case Step::Status::FINISHED_SUCCESS:
			return b_status;
		case Step::Status::UNFINISHED:
			return a_status;//TODO
		case Step::Status::FINISHED_FAILURE:
			nyi //TODO
		default:
			assert(0);
	} 
}

Toplevel::Goal Combo::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Combo::run(Run_info info,Toplevel::Goal goals){
	goals = step_a.run(info,goals);
	goals = step_b.run(info,goals);
	return goals;
}

unique_ptr<Step_impl> Combo::clone()const{
	return unique_ptr<Step_impl>(new Combo(*this));
}

bool Combo::operator==(Combo const& b)const{
	return step_a == b.step_a && step_b == b.step_b;
}

Turn_on_light::Turn_on_light():lights_goal({Lights::Loading_indicator::GEARS,true,Lights::Blinky_mode::NO_FLASH}){}

Step::Status Turn_on_light::done(Next_mode_info info){
	return (ready(status(info.status.lights),lights_goal)) ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
}

Toplevel::Goal Turn_on_light::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Turn_on_light::run(Run_info,Toplevel::Goal goals){
	goals.lights = lights_goal;
	return goals;
}

unique_ptr<Step_impl> Turn_on_light::clone()const{
	return unique_ptr<Step_impl>(new Turn_on_light(*this));
}

bool Turn_on_light::operator==(Turn_on_light const& b)const{
	return lights_goal == b.lights_goal;
}

void Score_gear::display(std::ostream& o)const{
	Step_impl_inner<Score_gear>::display(o);
	o<<"(";
	for(Step a: steps){
		a.display(o);
		o<<" ";
	}
	o<<")";
}

Score_gear::Score_gear():
	steps({
			Step{Lift_gear()},//lift the gear
			Step{Combo{ //slide the gear on the peg
				Step{Lift_gear()},
				Step{Ram{SCORE_GEAR_APPROACH_DIST}}
			}},
			Step{Drop_gear()}, //release the gear
			Step{Combo{ //back off
				Step{Drop_gear()},
				Step{Ram{-SCORE_GEAR_APPROACH_DIST}}
			}},
			Step{Drop_collector()}, // lower the collector to the floor
	}),
	stage(Stage::LIFT){}

Toplevel::Goal Score_gear::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Score_gear::run(Run_info info,Toplevel::Goal goals){
	if(stage == Stage::DONE) return goals;
	return steps[stage].run(info, goals);
}

bool Score_gear::operator==(Score_gear const& b)const{
	return steps == b.steps && stage == b.stage;
}

unique_ptr<Step_impl> Score_gear::clone()const{
	return unique_ptr<Step_impl>(new Score_gear(*this));
}
void Score_gear::advance(){
	stage = [&]{ //move onto next step
		switch(stage){
			case LIFT:
				return SCORE;
			case SCORE:
				return RELEASE;
			case RELEASE:
				return BACK_OFF;
			case BACK_OFF:
				return STOW;
			case STOW:
			case DONE:
				return DONE;
			default:
				assert(0);
		}
	}();
}

Step::Status Score_gear::done(Next_mode_info info){
	switch(steps[stage].done(info)){
		case Step::Status::UNFINISHED:
			break;
		case Step::Status::FINISHED_FAILURE: //treat a failure as a success
		case Step::Status::FINISHED_SUCCESS:
			advance();
			if(stage == Stage::DONE) return Step::Status::FINISHED_SUCCESS;
			break;
		default:
			assert(0);
	}
	return Step::Status::UNFINISHED;
}

Step_impl const& Step::get()const{
	assert(impl);
	return *impl.get();
}

Step::Step(Step const& a):impl(a.get().clone()){}

/*
to put in its own file:

*/
class Do_list:public Step_impl_inner<Do_list>{
	vector<Step> steps;
	size_t index;

	public:
	explicit Do_list(vector<Step>);

	virtual Toplevel::Goal run(Run_info,Toplevel::Goal);
	virtual Toplevel::Goal run(Run_info);
	virtual Step::Status done(Next_mode_info);
	//virtual unique_ptr<Step_impl> clone()const;
	//virtual void display(ostream&)const;
	bool operator==(Do_list const&)const;
};

Do_list::Do_list(vector<Step> v):steps(v),index(0){}

Toplevel::Goal Do_list::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Do_list::run(Run_info,Toplevel::Goal){
	nyi
}

bool Do_list::operator==(Do_list const& a)const{
	return steps==a.steps && index==a.index;
}

Step::Status Do_list::done(Next_mode_info){
	nyi
}

ostream& operator<<(ostream& o,Step_impl const& a){
	a.display(o);
	return o;
}
#ifdef STEP_TEST
void test_step(Step a){
	PRINT(a);
	auto b=a;
	assert(b==a);
	//see that can at least run "run", and "done"
	//a.run(example
	//TODO: try to visit all the possible states that you can get to from the initial one, and see that things don't fail
	set<Step> visited;
	queue<Step> to_visit;
	to_visit|=a;

	//might want to put some limit on the amount of state space that's used.
	//TODO: make a list of the inputs to try
	//first, run all the "run"s
	//second, run ready on all the different things and see if any of them change
	//abort if they do change

	while(to_visit.size()){
		auto f=to_visit.front();
		to_visit.pop();

		if(visited.find(f)!=visited.end()) continue;

		visited|=f;

		//TODO: Actually run some data on it.
	}

	//see that will at some point "done" can be set
}

template<typename T>
void test_step(T t){
	return test_step(Step{t});
}

int main(){
	/*Drive_straight a;
	Chain b{Step{a},Executive{Teleop{}}};
	Chain c{b};
	auto d=c.next_mode(example((Next_mode_info*)0));
	PRINT(c.step);
	PRINT(d);*/

	Do_list dl{{}};
	//PRINT(dl);
	test_step(dl);
}
#endif
