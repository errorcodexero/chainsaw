#ifndef SHOOTER_H
#define SHOOTER_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "../util/util.h"
#include "../util/countdown_timer.h"
#include "../util/quick.h"
#include "nop.h"

struct Shooter{
	struct Goal{
		double value;//percent power
		Goal();
		Goal(double);
	};
	
	using Status_detail = Nop::Status_detail;
	
	using Status = Status_detail;

	#define SHOOTER_INPUT_ITEMS(X)\
		X(bool,enabled)
	struct Input{
		#define X(A,B) A B;
		SHOOTER_INPUT_ITEMS(X)
		#undef X
	};

	struct Input_reader{
		Shooter::Input operator()(Robot_inputs const&)const;
		Robot_inputs operator()(Robot_inputs,Shooter::Input)const;
	};

	using Output = Goal;
	
	struct Output_applicator{
		Shooter::Output operator()(Robot_outputs const&)const;
		Robot_outputs operator()(Robot_outputs,Output)const;
	};

	struct Estimator{
		Shooter::Status_detail last;
		Shooter::Output last_output;
	
		Shooter::Status_detail get()const;
		void update(Time,Shooter::Input,Shooter::Output,std::ostream& = std::cerr);
		Estimator();
	};

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator;
};

std::ostream& operator<<(std::ostream&,Shooter::Goal);
std::ostream& operator<<(std::ostream&,Shooter::Input);
std::ostream& operator<<(std::ostream&,Shooter);

bool operator<(Shooter::Status_detail,Shooter::Status_detail);
bool operator==(Shooter::Status_detail,Shooter::Status_detail);
bool operator!=(Shooter::Status_detail,Shooter::Status_detail);

bool operator==(Shooter::Input,Shooter::Input);
bool operator!=(Shooter::Input,Shooter::Input);
bool operator<(Shooter::Input,Shooter::Input);

bool operator==(Shooter::Goal,Shooter::Goal);
bool operator!=(Shooter::Goal,Shooter::Goal);
bool operator<(Shooter::Goal,Shooter::Goal);

bool operator==(Shooter::Input_reader,Shooter::Input_reader);
bool operator<(Shooter::Input_reader,Shooter::Input_reader);

bool operator==(Shooter::Estimator,Shooter::Estimator);
bool operator!=(Shooter::Estimator,Shooter::Estimator);

bool operator==(Shooter::Output_applicator,Shooter::Output_applicator);

bool operator==(Shooter,Shooter);
bool operator!=(Shooter,Shooter);

std::set<Shooter::Input> examples(Shooter::Input*);
std::set<Shooter::Goal> examples(Shooter::Goal*);

Shooter::Status status(Shooter::Status_detail);
Shooter::Output control(Shooter::Status_detail,Shooter::Goal);
bool ready(Shooter::Status,Shooter::Goal);

#endif

