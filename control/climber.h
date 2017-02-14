#ifndef	CLIMBER_H
#define CLIMBER_H

#include <set>
#include "../util/interface.h"
#include "nop.h"
#include "../util/countdown_timer.h"

struct Climber{
	enum class Goal{AUTO_CLIMB,CLIMB,STOP,RELEASE,AUTO_RELEASE};
	
	enum class Output{CLIMB,STOP,RELEASE};

	using Input = Nop::Input;

	/*struct Input{//no sensors?
		bool climbed, released;//hall-effects
		Input();
		Input(bool,bool);
	};*/

	enum class Status_detail{ERROR_,CLIMBED,CLIMBING,STOPPED,RELEASING,RELEASED};//TODO: if no sensors: remove ERROR_
	
	typedef Status_detail Status;
	
	using Input_reader = Nop::Input_reader;

	/*struct Input_reader{
		Input operator()(Robot_inputs const&)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};*/

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output)const;
		Output operator()(Robot_outputs const&)const;
	};

	struct Estimator{
		Status_detail last;
		Output last_output;
		Countdown_timer state_timer;
		
		void update(Time,Input,Output);
		Status_detail get()const;
		Estimator();
	};

	Input_reader input_reader;
	Output_applicator output_applicator;
	Estimator estimator;
};

std::set<Climber::Goal> examples(Climber::Goal*);
std::set<Climber::Input> examples(Climber::Input*);
std::set<Climber::Status_detail> examples(Climber::Status_detail*);
std::set<Climber::Output> examples(Climber::Output*);

std::ostream& operator<<(std::ostream&,Climber::Goal);
//std::ostream& operator<<(std::ostream&,Climber::Input);
std::ostream& operator<<(std::ostream&,Climber::Status_detail);
std::ostream& operator<<(std::ostream&,Climber::Output);
std::ostream& operator<<(std::ostream&,Climber const&);
/*
bool operator<(Climber::Input,Climber::Input);
bool operator==(Climber::Input,Climber::Input);
bool operator!=(Climber::Input,Climber::Input);
*/
bool operator==(Climber::Estimator,Climber::Estimator);
bool operator!=(Climber::Estimator,Climber::Estimator);

bool operator==(Climber,Climber);
bool operator!=(Climber,Climber);

Climber::Output control(Climber::Status,Climber::Goal);
Climber::Status status(Climber::Status);
bool ready(Climber::Status,Climber::Goal);

#endif
