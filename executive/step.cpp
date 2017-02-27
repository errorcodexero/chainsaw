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

static const Inch WIDTH_OF_ROBOT = 26.0;//inches, note: actual distance between centers of both wheels is 27.0 in, but 26 results in more precise turning  //TODO: finds some way of dealing with constants like this and wheel diameter

Turn::Turn(Rad a):target_angle(a),initial_distances({0,0}),init(false),side_goals({0,0}){
	Inch side_goal = target_angle * 0.5 * WIDTH_OF_ROBOT;
	side_goals = Drivebase::Distances{side_goal,-side_goal};
	motion_profile = {side_goal,0.1,0.25};
}

Toplevel::Goal Turn::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Turn::run(Run_info info,Toplevel::Goal goals){
	if(!init){
		initial_distances = info.status.drive.distances;
		init = true;
	}
	Drivebase::Distances distance_travelled = info.status.drive.distances - initial_distances;
	
	double power = motion_profile.target_speed(mean(distance_travelled.l,-distance_travelled.r)); 
	goals.drive.left = power;
	goals.drive.right = -power;
	goals.shifter = Gear_shifter::Goal::LOW;
	return goals;
}

bool Turn::done(Next_mode_info info){
	static const Inch TOLERANCE = 3.0;//inches
	Drivebase::Distances distance_travelled = info.status.drive.distances - initial_distances;
	Drivebase::Distances distance_left = fabs(side_goals - distance_travelled);
	if(mean(distance_left.l,distance_left.r) < TOLERANCE){
		 in_range.update(info.in.now,info.in.robot_mode.enabled);
	} else {
		static const Time FINISH_TIME = 1.0;//seconds
		in_range.set(FINISH_TIME);
	}
	return in_range.done();
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

bool Step::done(Next_mode_info a){
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

Drive_straight::Drive_straight(Inch goal):target_dist(goal),initial_distances(Drivebase::Distances{0,0}),init(false),motion_profile(goal,0.02,.5),gear(Gear_shifter::Goal::LOW){}//Motion profiling values from testing
Drive_straight::Drive_straight(Inch goal,double vel_modifier,double max):Drive_straight(goal){
	motion_profile = {goal,vel_modifier,max};
}

Drivebase::Distances Drive_straight::get_distance_travelled(Drivebase::Distances current){
	Drivebase::Distances distance_travelled = current - initial_distances;
	cout<<"\nbefore:"<<distance_travelled;
	distance_travelled.r += distance_travelled.r * RIGHT_DISTANCE_CORRECTION;//right encoder would read that it's moved less than it has without error correction
	cout<<"    after:"<<distance_travelled<<"\n";
	return distance_travelled;
}

bool Drive_straight::done(Next_mode_info info){
	static const Inch TOLERANCE = 3.0;//inches
	Drivebase::Distances distance_travelled = get_distance_travelled(info.status.drive.distances);
	Drivebase::Distances distance_left = (Drivebase::Distances{target_dist,target_dist} - distance_travelled);
	if(fabs(mean(distance_left.l,distance_left.r)) < TOLERANCE){
		in_range.update(info.in.now,info.in.robot_mode.enabled);
	} else {
		static const Time FINISH_TIME = 1.0;
		in_range.set(FINISH_TIME);
	}
	cout<<"\ndistance_travelled:"<<distance_travelled<<"  mean:"<<mean(distance_travelled.l,distance_travelled.r)<<"   goal:"<<target_dist<<"   in_range:"<<in_range<<"\n";
	return in_range.done();
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

	double power = target_to_out_power(motion_profile.target_speed(mean(distance_travelled.l,distance_travelled.r)));
	
	//cout<<"\ndistance_travelled:"<<distance_travelled<<"  mean:"<<mean(distance_travelled.l,distance_travelled.r)<<"   goal:"<<target_dist<<"     power:"<<power<<"    in_range:"<<in_range<<"\n";
	
	goals.drive.left = clip(power);
	goals.drive.right = clip(power - power * RIGHT_SPEED_CORRECTION);//right side would go faster than the left without error correction
	goals.shifter = gear;
	return goals;
}

unique_ptr<Step_impl> Drive_straight::clone()const{
	return unique_ptr<Step_impl>(new Drive_straight(*this));
}

bool Drive_straight::operator==(Drive_straight const& b)const{
	return target_dist == b.target_dist && initial_distances == b.initial_distances && init == b.init && motion_profile == b.motion_profile && in_range == b.in_range && gear == b.gear;
}

Align::Align(double a):mode(Align::Mode::NONVISION),blocks({}),current(0),center(0),angle_estimate(a),nonvision_align(Step{Turn(a)}){}
Align::Align():Align(0){}

void Align::update(Camera camera){
	if(!camera.enabled || camera.blocks.size() == 0){
		mode = Mode::NONVISION;
		blocks = {};
		current = 0;
		center = 0;
	} else {
		mode = Mode::VISION;
		blocks = camera.blocks;
		//TODO: check for two largest blocks that are in the expected y range
		current = mean(blocks[0].x,blocks[1].x);
		center = mean(blocks[0].min_x,blocks[1].max_x);
	}
}

bool Align::done(Next_mode_info info){
	update(info.in.camera);
	switch(mode){
		case Mode::VISION:
			{
				const int TOLERANCE= 2;
				if(current > center - TOLERANCE && current < center + TOLERANCE){
					in_range.update(info.in.now,info.in.robot_mode.enabled);
				} else {
					static const Time FINISH_TIME = 1.0;
					in_range.set(FINISH_TIME);
				}
				return in_range.done();
			}
		case Mode::NONVISION:
			{
				if(!in_range.done()){
					in_range.update(info.in.now,info.in.robot_mode.enabled);
				}else{
					static const Time FINISH_TIME = 1.0;
					in_range.set(FINISH_TIME);
				}
				return nonvision_align.done(info);
			}
		default:
			assert(0);
	}
}

Toplevel::Goal Align::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Align::run(Run_info info,Toplevel::Goal goals){
	update(info.in.camera);
	cout << "Align:    " << blocks[0] << "," << blocks[1] << "   " << current << "   " << center << "\n";
	goals.shifter = Gear_shifter::Goal::LOW;
	switch(mode){
		case Mode::VISION:
			{
				const double power = .1;
				const int TOLERANCE = 3.0;
				if(current < center - TOLERANCE){
					goals.drive.left = power;
					goals.drive.right = -power;
				} else if(current > center + TOLERANCE){
					goals.drive.left = -power;
					goals.drive.right = power;
				} else {
					goals.drive.left = 0;
					goals.drive.right = 0;
				}
				return goals;
			}
		case Mode::NONVISION:
			return nonvision_align.run(info,goals);
		default:
			assert(0);
	}
}

unique_ptr<Step_impl> Align::clone()const{
	return unique_ptr<Step_impl>(new Align());
}

bool Align::operator==(Align const& b)const{
	return mode == b.mode && blocks == b.blocks && current == b.current && center == b.center && in_range == b.in_range && angle_estimate == b.angle_estimate && nonvision_align == b.nonvision_align;
}

Wait::Wait(Time wait_time){
	wait_timer.set(wait_time);
}

bool Wait::done(Next_mode_info){
	return wait_timer.done();
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

Lift_gear::Lift_gear():gear_goal({Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::UP}),ball_goal({Intake::Goal::OFF,Arm::Goal::IN,Ball_lifter::Goal::OFF}){}

bool Lift_gear::done(Next_mode_info info){
	return ready(status(info.status.gear_collector),gear_goal) && ready(status(info.status.collector),ball_goal);
}

Toplevel::Goal Lift_gear::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Lift_gear::run(Run_info info, Toplevel::Goal goals){
	(void)info;
	goals.gear_collector = gear_goal;
	goals.collector = ball_goal;
	return goals;
}

unique_ptr<Step_impl> Lift_gear::clone()const{
	return unique_ptr<Step_impl>(new Lift_gear(*this));
}

bool Lift_gear::operator==(Lift_gear const& b)const{
	(void)b;
	return true;
}

Drop_gear::Drop_gear():gear_goal({Gear_grabber::Goal::OPEN,Gear_lifter::Goal::UP}),ball_goal({Intake::Goal::OFF,Arm::Goal::IN,Ball_lifter::Goal::OFF}){}

bool Drop_gear::done(Next_mode_info info){	
	return ready(status(info.status.gear_collector),gear_goal) && ready(status(info.status.collector),ball_goal);
}

Toplevel::Goal Drop_gear::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Drop_gear::run(Run_info info,Toplevel::Goal goals){
	(void)info;
	goals.gear_collector = gear_goal;
	goals.collector = ball_goal;
	return goals;
}

unique_ptr<Step_impl> Drop_gear::clone()const{
	return unique_ptr<Step_impl>(new Drop_gear(*this));
}

bool Drop_gear::operator==(Drop_gear const& b)const{
	(void)b;
	return true;
}

Drop_collector::Drop_collector():gear_goal({Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::DOWN}),ball_goal({Intake::Goal::OFF,Arm::Goal::IN,Ball_lifter::Goal::OFF}){}

bool Drop_collector::done(Next_mode_info info){
	return ready(status(info.status.gear_collector),gear_goal) && ready(status(info.status.collector),ball_goal);
}

Toplevel::Goal Drop_collector::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Drop_collector::run(Run_info info,Toplevel::Goal goals){
	(void)info;
	goals.gear_collector = gear_goal;
	goals.collector = ball_goal;
	return goals;
}

unique_ptr<Step_impl> Drop_collector::clone()const{
	return unique_ptr<Step_impl>(new Drop_collector(*this));
}

bool Drop_collector::operator==(Drop_collector const& b)const{
	(void)b;
	return true;
}

Combo::Combo(Step a,Step b):step_a(a),step_b(b){}

bool Combo::done(Next_mode_info info){
	return step_a.done(info) && step_b.done(info);
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
	virtual bool done(Next_mode_info);
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

bool Do_list::done(Next_mode_info){
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
