#ifndef BALL_COOLECTOR_LIFT_H
#define BALL_COLLECTOR_LIFT_H

#include "../util/countdown_timer.h"
#include "nop.h"

struct Ball_collector_lift{
	using Input=Nop::Input;
	using Input_reader=Nop::Input_reader;
	Input_reader input_reader;

	#define BALL_COLLECTOR_LIFT_OUTPUTS(X) X(UP) X(DOWN)
	#define X(A) A,
	enum class Output{BALL_COLLECTOR_LIFT_OUTPUTS(X)};
	#undef X

	struct Output_applicator{
		int solenoid;

		explicit Output_applicator(int);

		Robot_outputs operator()(Robot_outputs,Output)const;
		Output operator()(Robot_outputs)const;
	};
	Output_applicator output_applicator;

	#define BALL_COLLECTOR_LIFT_GOALS(X) X(UP) X(DOWN) X(X)
	#define X(A) A,
	enum class Goal{BALL_COLLECTOR_LIFT_GOALS(X)};
	#undef X

	#define BALL_COLLECTOR_LIFT_STATUSES(X) X(UP) X(DOWN) X(GOING_UP) X(GOING_DOWN)
	#define X(A) A,
	enum class Status{BALL_COLLECTOR_LIFT_STATUSES(X)};
	#undef X

	using Status_detail=Status;

	class Estimator{
		Countdown_timer timer;
		Output last_output;

		public:
		Estimator();
		void update(Time,Input,Output);	
		Status_detail get()const;
	};
	Estimator estimator;

	Ball_collector_lift();
};

std::ostream& operator<<(std::ostream& o,Ball_collector_lift const&);

std::set<Ball_collector_lift::Goal> examples(const Ball_collector_lift::Goal*);
std::ostream& operator<<(std::ostream&,Ball_collector_lift::Goal);

std::set<Ball_collector_lift::Output> examples(const Ball_collector_lift::Output*);
std::ostream& operator<<(std::ostream&,Ball_collector_lift::Output);

std::set<Ball_collector_lift::Status_detail> examples(const Ball_collector_lift::Status_detail*);
std::ostream& operator<<(std::ostream&,Ball_collector_lift::Status_detail);

Ball_collector_lift::Output control(Ball_collector_lift::Status_detail,Ball_collector_lift::Goal);
bool ready(Ball_collector_lift::Status,Ball_collector_lift::Goal);
Ball_collector_lift::Status status(Ball_collector_lift::Status_detail);

#endif
