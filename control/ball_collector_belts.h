#ifndef BALL_COLLECTOR_BELTS_H
#define BALL_COLLECTOR_BELTS_H

#include "nop.h"

struct Ball_collector_belts{
	using Input=Nop::Input;
	using Input_reader=Nop::Input_reader;
	Input_reader input_reader;

	#define BALL_COLLECTOR_BELTS_OUTPUTS(X) X(IN) X(OUT) X(STOP)
	#define X(A) A,
	enum class Output{BALL_COLLECTOR_BELTS_OUTPUTS(X)};
	#undef X

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output)const;
		Output operator()(Robot_outputs)const;
	};
	Output_applicator output_applicator;

	using Goal=Output;

	using Status_detail=Nop::Status_detail;
	using Status=Status_detail;

	using Estimator=Nop::Estimator;
	Estimator estimator;
};

std::ostream& operator<<(std::ostream&,Ball_collector_belts const&);
Ball_collector_belts::Output control(Ball_collector_belts::Status_detail,Ball_collector_belts::Goal);
bool ready(Ball_collector_belts::Status,Ball_collector_belts::Goal);

#endif
