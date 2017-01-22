#ifndef GEAR_GRASPER_H
#define GEAR_GRASPER_H

#include "nop.h"
#include "../util/countdown_timer.h"

struct Gear_grasper{
	using Input=Nop::Input;
	using Input_reader=Nop::Input_reader;
	Input_reader input_reader;

	/*using Output=Nop::Output;
	using Output_applicator=Nop::Output_applicator;*/

	#define GEAR_GRASPER_OUTPUTS(X) X(OPEN) X(CLOSE)
	enum class Output{
		#define X(A) A,
		GEAR_GRASPER_OUTPUTS(X)
		#undef X
	};

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output)const;
		Output operator()(Robot_outputs)const;
	};

	Output_applicator output_applicator;

	//using Status_detail=Nop::Status_detail;
	#define GEAR_GRASPER_STATUSES(X) X(OPEN) X(CLOSED) X(OPENING) X(CLOSING)
	#define X(A) A, 
	enum class Status_detail{GEAR_GRASPER_STATUSES(X)};
	#undef X

	using Status=Status_detail;

	//using Estimator=Nop::Estimator_nop<Status_detail>;
	class Estimator{
		Output last_output;
		Countdown_timer until_finish;

		public:
		Estimator();
		void update(Time,Input,Output);
		Status_detail get()const;
	};
	Estimator estimator;

	//using Goal=Nop::Goal;
	#define GEAR_GRASPER_GOALS(A) A(OPEN) A(CLOSE) A(X)
	#define X(A) A,
	enum class Goal{GEAR_GRASPER_GOALS(X)};
	#undef X
};

std::ostream& operator<<(std::ostream&,Gear_grasper const&);

std::set<Gear_grasper::Goal> examples(const Gear_grasper::Goal*);
std::ostream& operator<<(std::ostream&,Gear_grasper::Goal);

std::set<Gear_grasper::Output> examples(const Gear_grasper::Output*);
std::ostream& operator<<(std::ostream&,Gear_grasper::Output);

Gear_grasper::Output control(Gear_grasper::Status_detail,Gear_grasper::Goal);
bool ready(Gear_grasper::Status,Gear_grasper::Goal);

#endif
