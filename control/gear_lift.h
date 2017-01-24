#ifndef GEAR_LIFT_H
#define GEAR_LIFT_H

#include "ball_collector_lift.h"

struct Gear_lift{
	using Input=Ball_collector_lift::Input;
	using Input_reader=Ball_collector_lift::Input_reader;
	Input_reader input_reader;

	using Output=Ball_collector_lift::Output;
	using Output_applicator=Ball_collector_lift::Output_applicator;
	Output_applicator output_applicator;

	using Status_detail=Ball_collector_lift::Status_detail;
	using Status=Ball_collector_lift::Status;
	using Estimator=Ball_collector_lift::Estimator;
	Estimator estimator;

	using Goal=Ball_collector_lift::Goal;

	Gear_lift();
};

std::ostream& operator<<(std::ostream&,Gear_lift const&);

#endif
