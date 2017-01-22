#ifndef AGITATOR_H
#define AGITATOR_H

#include "nop.h"

struct Agitator{
	using Input=Nop::Input;
	using Input_reader=Nop::Input_reader;
	Input_reader input_reader;

	using Output=bool;

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output)const;
		Output operator()(Robot_outputs)const;
	};
	Output_applicator output_applicator;

	using Status=Nop::Status;
	using Status_detail=Nop::Status_detail;
	using Estimator=Nop::Estimator;
	Estimator estimator;

	using Goal=Output;
};

std::ostream& operator<<(std::ostream&,Agitator const&);
bool ready(Agitator::Status,Agitator::Goal);

#endif
