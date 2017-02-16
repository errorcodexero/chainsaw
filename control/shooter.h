#ifndef SHOOTER_H
#define SHOOTER_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "../util/util.h"
#include "../util/countdown_timer.h"
#include "../util/quick.h"

struct Shooter{
	struct Goal{
		PID_values constants;

		#define SHOOTER_MODES X(VOLTAGE) X(SPEED)
		enum class Mode{
			#define X(name) name,
			SHOOTER_MODES
			#undef X
		};
		Mode mode;
		double value;//0-1 for voltage mode, RPM for speed mode
		Goal();
		Goal(PID_values,Shooter::Goal::Mode,double);
	};
	
	struct Status_detail{
		double speed;//rpm
		
		Status_detail();
		Status_detail(double);
	};
	
	using Status = Status_detail;

	//speed is in RPM
	#define SHOOTER_INPUT_ITEMS(X)\
		X(int,speed)\
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

	using Output = Talon_srx_output;
	
	struct Output_applicator{
		Shooter::Output operator()(Robot_outputs const&)const;
		Robot_outputs operator()(Robot_outputs,Output)const;
	};

	struct Estimator{
		Shooter::Status_detail last;
		Shooter::Output last_output;
	
		Shooter::Status_detail get()const;
		void update(Time,Shooter::Input,Shooter::Output);
		Estimator();
	};

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator;
};

std::ostream& operator<<(std::ostream&,Shooter::Goal::Mode);
std::ostream& operator<<(std::ostream&,Shooter::Goal);
std::ostream& operator<<(std::ostream&,Shooter::Input);
std::ostream& operator<<(std::ostream&,Shooter::Status_detail);
std::ostream& operator<<(std::ostream&,Shooter);

bool operator==(Shooter::Input,Shooter::Input);
bool operator!=(Shooter::Input,Shooter::Input);
bool operator<(Shooter::Input,Shooter::Input);

bool operator<(Shooter::Status_detail,Shooter::Status_detail);
bool operator==(Shooter::Status_detail,Shooter::Status_detail);
bool operator!=(Shooter::Status_detail,Shooter::Status_detail);

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
std::set<Shooter::Status_detail> examples(Shooter::Status_detail*);
std::set<Shooter::Status> examples(Shooter::Status*);

Shooter::Output control(Shooter::Status_detail,Shooter::Goal);
Shooter::Status status(Shooter::Status_detail);
bool ready(Shooter::Status,Shooter::Goal);

#endif

