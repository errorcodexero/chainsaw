#include "step.h"
#include<queue>
#include "executive.h"
#include "../util/util.h"
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

Toplevel::Goal Step::run(Run_info info){
	return impl->run(info);
}

static const Inch WIDTH_OF_ROBOT = 30;//inches //TODO: make more accurate //TODO: finds some way of dealing with constants like this and wheel diameter

Turn::Turn(Rad a):target_angle(a),initial_distances({0,0}),init(false),side_goals({0,0}){
	Inch side = target_angle * 0.5 * WIDTH_OF_ROBOT;
	side_goals = Drivebase::Distances{side,-side};	
}

Toplevel::Goal Turn::run(Run_info info){
	if(!init){
		initial_distances = info.status.drive.distances;
		init = true;
	}
	Toplevel::Goal goals;
	Motion_profile mp1 = {mean(side_goals.l,-side_goals.r),0.1,0.25};
	double power = mp1.target_speed(mean(info.status.drive.distances.l,-info.status.drive.distances.r)); 
	goals.drive.left = power;
	goals.drive.right = -power;
	return goals;
}

bool Turn::done(Next_mode_info info){
	static const Inch TOLERANCE = 3.0;//inches
	Drivebase::Distances distance_travelled = info.status.drive.distances - initial_distances;
	Drivebase::Distances distance_left = fabs(side_goals - distance_travelled);
	cout<<"\nSTART:"<<initial_distances<<"     NOW:"<<info.status.drive.distances<<"     TRAVELLED:"<<distance_travelled<<"    LEFT:"<<distance_left<<"    GOALS:"<<side_goals<<"    in_range:"<<in_range<<"\n";
	if(mean(distance_left.l,distance_left.r) < TOLERANCE){
		in_range.update(info.in.now,info.in.robot_mode.enabled);
	} else {
		static const Time FINISH_TIME = 1.0;
		in_range.set(FINISH_TIME);
	}
	return in_range.done();
}

std::unique_ptr<Step_impl> Turn::clone()const{
	return unique_ptr<Step_impl>(new Turn(*this));
}

bool Turn::operator==(Turn const& b)const{
	return target_angle == b.target_angle && initial_distances == b.initial_distances && in_range == b.in_range;
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

Drive_straight::Drive_straight(Inch goal):target_dist(goal),initial_distances(Drivebase::Distances{0,0}),init(false),motion_profile(goal,0.1,.5){}//Motion profiling values from testing

bool Drive_straight::done(Next_mode_info info){	static const Inch TOLERANCE = 3.0;//inches
	Drivebase::Distances distance_travelled = info.status.drive.distances - initial_distances;
	Drivebase::Distances distance_left = fabs(Drivebase::Distances{target_dist,target_dist} - distance_travelled);
	if(mean(distance_left.l,distance_left.r) < TOLERANCE){
		in_range.update(info.in.now,info.in.robot_mode.enabled);
	} else {
		static const Time FINISH_TIME = 1.0;
		in_range.set(FINISH_TIME);
	}
	return in_range.done();
}

Toplevel::Goal Drive_straight::run(Run_info info){
	if(!init){
		initial_distances = info.status.drive.distances;
		init = true;
	}
	Toplevel::Goal goals;

	double power = mean(motion_profile.target_speed(info.status.drive.distances.l),motion_profile.target_speed(info.status.drive.distances.r));
	goals.drive.left = power;
	goals.drive.right = power;
	goals.shifter = Gear_shifter::Goal::LOW;
	return goals;
}

unique_ptr<Step_impl> Drive_straight::clone()const{
	return unique_ptr<Step_impl>(new Drive_straight(*this));
}

bool Drive_straight::operator==(Drive_straight const& b)const{
	return target_dist == b.target_dist && initial_distances == b.initial_distances && init == b.init && motion_profile == b.motion_profile && in_range == b.in_range;
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

	virtual Toplevel::Goal run(Run_info);
	virtual bool done(Next_mode_info);
	//virtual unique_ptr<Step_impl> clone()const;
	//virtual void display(ostream&)const;
	bool operator==(Do_list const&)const;
};

Do_list::Do_list(vector<Step> v):steps(v),index(0){}

Toplevel::Goal Do_list::run(Run_info){
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
