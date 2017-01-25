#ifndef LIFT_H
#define LIFT_H

struct Lift{
	enum class Goal{UP, DOWN, OFF};
	using Input=Nop::Input;
	typedef Input Status_detail;
	typedef Status_detail Status;
	using Input_reader=Nop:: Input_reader;
	typedef Goal Output;
	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Intake::Output)const;
		Lift::Output operator()(Robot_outputs)const;
	};
	using Estimator=Nop::Estimator;

	Input_reader input_reader;
	Estimator estimator;
	output applicator output_applicator;
};




#endif
