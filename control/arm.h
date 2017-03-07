#ifndef	ARM_H
#define ARM_H

#include <set>
#include "../util/interface.h"
#include "../util/countdown_timer.h"

struct Arm{
	#define ARM_GOALS(F) F(STOW) F(LOW) F(X)
	enum class Goal{
		#define X(A) A,
		ARM_GOALS(X)
		#undef X
	};
	
	#define ARM_OUTPUTS(X) X(STOW) X(LOW)
	enum class Output{
		#define X(A) A,
		ARM_OUTPUTS(X)
		#undef X
	};

	struct Input{
		bool enabled;
		Input();
		Input(bool);
	};

	enum class Status_detail{STOW,STOWING,LOWERING,LOW};
	
	typedef Status_detail Status;
	
	struct Input_reader{
		Input operator()(Robot_inputs const&)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output)const;
		Output operator()(Robot_outputs const&)const;
	};

	struct Estimator{
		Status_detail last;
		Countdown_timer state_timer;		

		void update(Time,Input,Output,std::ostream& = std::cerr);
		Status_detail get()const;
		Estimator();
	};

	Input_reader input_reader;
	Output_applicator output_applicator;
	Estimator estimator;
};

std::set<Arm::Goal> examples(Arm::Goal*);
std::set<Arm::Input> examples(Arm::Input*);
std::set<Arm::Output> examples(Arm::Output*);
std::set<Arm::Status_detail> examples(Arm::Status_detail*);

std::ostream& operator<<(std::ostream&,Arm::Goal);
std::ostream& operator<<(std::ostream&,Arm::Input);
std::ostream& operator<<(std::ostream&,Arm::Output);
std::ostream& operator<<(std::ostream&,Arm::Status_detail);
std::ostream& operator<<(std::ostream&,Arm const&);

bool operator<(Arm::Input,Arm::Input);
bool operator==(Arm::Input,Arm::Input);
bool operator!=(Arm::Input,Arm::Input);

bool operator==(Arm::Estimator,Arm::Estimator);
bool operator!=(Arm::Estimator,Arm::Estimator);

bool operator==(Arm,Arm);
bool operator!=(Arm,Arm);

Arm::Output control(Arm::Status,Arm::Goal);
Arm::Status status(Arm::Status);
bool ready(Arm::Status,Arm::Goal);

#endif
