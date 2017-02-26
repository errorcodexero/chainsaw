#ifndef STEP_H
#define STEP_H

#include "executive.h"
#include "../util/motion_profile.h"

struct Step_impl;
enum class Camera_con{ENABLE,DISABLED,NONVISION};
class Step{
	std::unique_ptr<Step_impl> impl;

	public:
	explicit Step(Step_impl const&);
	Step(Step const&);

	Toplevel::Goal run(Run_info,Toplevel::Goal);
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	Step_impl const& get()const;
	void display(std::ostream&)const;
	bool operator==(Step const&)const;
	bool operator<(Step const&)const;
};

std::ostream& operator<<(std::ostream&,Step const&);

struct Step_impl{
	virtual ~Step_impl();

	virtual Toplevel::Goal run(Run_info,Toplevel::Goal)=0;
	virtual Toplevel::Goal run(Run_info)=0;
	virtual bool done(Next_mode_info)=0;//could try to make this const.
	virtual std::unique_ptr<Step_impl> clone()const=0;
	virtual void display(std::ostream&)const;
	virtual bool operator<(Step const&)const=0;
	virtual bool operator==(Step const&)const=0;
};

template<typename T>
struct Step_impl_inner:Step_impl{
	std::unique_ptr<Step_impl> clone()const{
		T const& t=dynamic_cast<T const&>(*this);
		return std::unique_ptr<Step_impl>(new T(t));
	}
	
	void display(std::ostream& o)const{
		o<<type(*(T*)this);
	}

	bool operator==(Step const& a)const{
		//Step_impl const& b=a.get();
		//if(type_index(typeid(*this))!=type_index
		T const& t=dynamic_cast<T const&>(a.get());
		return operator==(t);
	}

	virtual bool operator==(T const&)const=0;

	bool operator<(Step const&)const{
		nyi
	}
};

using Inch=double;

class Drive_straight:public Step_impl_inner<Drive_straight>{//Drives straight a certain distance
	Inch target_dist;
	Drivebase::Distances initial_distances;
	bool init;
	Motion_profile motion_profile;
	Countdown_timer in_range;
	Gear_shifter::Goal gear;
	const double RIGHT_SPEED_CORRECTION = 0.05;//left and right sides of the robot drive at different speeds given the same power, left encoder gives us the actual distance, right is ~6% behind, always works pretty well
	
	Drivebase::Distances get_distance_travelled(Drivebase::Distances);//TODO: do this better

	public:
	explicit Drive_straight(Inch);
	explicit Drive_straight(Inch,double,double);

	Toplevel::Goal run(Run_info,Toplevel::Goal);
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	std::unique_ptr<Step_impl> clone()const;
	bool operator==(Drive_straight const&)const;
};

class Wait: public Step_impl_inner<Wait>{//Either stops all operation for a given period of time or continues to run the same goals for that time
	Countdown_timer wait_timer;//seconds
	public:
	explicit Wait(Time);

	Toplevel::Goal run(Run_info,Toplevel::Goal);
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	std::unique_ptr<Step_impl> clone()const;
	bool operator==(Wait const&)const;
};

class Lift_gear: public Step_impl_inner<Lift_gear>{//Closes the gear grabber and raises the gear collector to peg height
	Gear_collector::Goal gear_goal;//is the same in every one
	Collector::Goal ball_goal;//is the same in every one
	public:
	explicit Lift_gear();

	Toplevel::Goal run(Run_info,Toplevel::Goal);
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	std::unique_ptr<Step_impl> clone()const;
	bool operator==(Lift_gear const&)const;
};

class Drop_gear: public Step_impl_inner<Drop_gear>{//Opens the gear grabber but keeps the manipulator at peg height
	Gear_collector::Goal gear_goal;//is the same in every one
	Collector::Goal ball_goal;//is the same in every one

	public:
	explicit Drop_gear();

	Toplevel::Goal run(Run_info,Toplevel::Goal);
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	std::unique_ptr<Step_impl> clone()const;
	bool operator==(Drop_gear const&)const;
};

class Drop_collector: public Step_impl_inner<Drop_collector>{//Lowers the gear manipulator to the floor
	Gear_collector::Goal gear_goal;//is the same in every one
	Collector::Goal ball_goal;//is the same in every one
	
	public:
	explicit Drop_collector();

	Toplevel::Goal run(Run_info,Toplevel::Goal);
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	std::unique_ptr<Step_impl> clone()const;
	bool operator==(Drop_collector const&)const;
};

class Combo: public Step_impl_inner<Combo>{//Runs two steps at the same time
	Step step_a;
	Step step_b;
	public:
	explicit Combo(Step,Step);//the second step will overwrite goals from the first one if they both modify the same parts of the robot

	Toplevel::Goal run(Run_info,Toplevel::Goal);
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	std::unique_ptr<Step_impl> clone()const;
	bool operator==(Combo const&)const;
};

struct Turn: Step_impl_inner<Turn>{//orients the robot to a certain angle relative to its starting orientation
	Rad target_angle;//radians,clockwise=positive
	Drivebase::Distances initial_distances;
	bool init;
	Drivebase::Distances side_goals;
	Motion_profile motion_profile;
	Countdown_timer in_range;
	const double RIGHT_SPEED_CORRECTION = 0.05;
	const double RIGHT_DISTANCE_CORRECTION = 0.08;

	Drivebase::Distances get_distance_travelled(Drivebase::Distances);

	explicit Turn(Rad);
	Toplevel::Goal run(Run_info,Toplevel::Goal);
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	std::unique_ptr<Step_impl> clone()const;
	bool operator==(Turn const&)const;
};
struct Align: public Step_impl_inner<Align>{
	std::vector<Pixy::Block> blocks;
	int current;
	int center;
	bool manualflag;
	bool firsttime;
	Camera_con camera_con;
	Countdown_timer in_range; 
	public:
	explicit Align();

	Toplevel::Goal run(Run_info,Toplevel::Goal);//TODO
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	std::unique_ptr<Step_impl> clone()const;
	bool operator==(Align const&)const;
};

#endif
