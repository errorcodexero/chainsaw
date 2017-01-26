#ifndef GEAR_SHIFTER_H
#define GEAR_SHIFTER_H

struct Gear_shifter{
	#define GEAR_SHIFTER_GOALS X(AUTO) X(LOW) X(HIGH)
	enum class Goal{
		#define X(A) A,
		GEAR_SHIFTER_GOALS
		#undef X
	}

	using Input=Drivebase::Input;
	using Input_reader=Drivebase::Input_reader;
	
	#define GEAR_SHIFTER_OUTPUTS X(LOW) X(HIGH)
	enum class Output{
		#define X(A) A,
		GEAR_SHIFTER_OUTPUTS
		#undef X
	}
	
	using Status=Nop::Status;
	using Status_detail=Output;

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output);
		Output operator()(Robot_outputs);
	}

	struct Estimator{
		Estimator();
		Status_detail get()const;
		void update(Time,Input,Output);
	}

	Input_reader input_reader;
	Output_applicator output_applicator;
	Estimator estimator;
};

set<Gear_shifter::Goal> examples(Gear_shifter::Goal*);
set<Gear_shifter::Output> examples(Gear_shifter::Output*);

#endif	
