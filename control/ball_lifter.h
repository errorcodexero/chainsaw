#ifndef BALL_LIFTER_H
#define BALL_LIFTER_H
#include "nop.h"

struct Ball_lifter{
	enum class Goal{UP, DOWN, OFF};
	using Input=Nop::Input;
	typedef Input Status_detail;
	typedef Status_detail Status;
	using Input_reader=Nop::Input_reader;
	typedef Goal Output;
	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Ball_lifter::Output)const;
		Ball_lifter::Output operator()(Robot_outputs)const;
	};
	using Estimator=Nop::Estimator;

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator;
};


std::ostream& operator<<(std::ostream&,Ball_lifter::Goal);
std::ostream& operator<<(std::ostream&,Ball_lifter);

bool operator<(Ball_lifter::Status_detail,Ball_lifter::Status_detail);
bool operator==(Ball_lifter::Status_detail,Ball_lifter::Status_detail);
bool operator!=(Ball_lifter::Status_detail,Ball_lifter::Status_detail);

bool operator==(Ball_lifter::Output_applicator,Ball_lifter::Output_applicator);

bool operator==(Ball_lifter,Ball_lifter);
bool operator!=(Ball_lifter,Ball_lifter);

std::set<Ball_lifter::Goal> examples(Ball_lifter::Goal*);
std::set<Ball_lifter::Status_detail> examples(Ball_lifter::Status_detail*);

Ball_lifter::Output control(Ball_lifter::Status_detail, Ball_lifter::Goal);
Ball_lifter::Status status(Ball_lifter::Status_detail);
bool ready(Ball_lifter::Status, Ball_lifter::Goal);


#endif
