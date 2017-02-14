#ifndef SHOOTER_FEED_H
#define SHOOTER_FEED_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "nop.h"

struct Shooter_feed{
	enum class Goal{IN,OFF,OUT};
	
	using Input=Nop::Input;
	
	using Status_detail = Nop::Status_detail;

	using Status = Nop::Status;

	using Input_reader=Nop::Input_reader;

	using Output = Goal;
	
	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Shooter_feed::Output)const;
		Shooter_feed::Output operator()(Robot_outputs)const;	
	};

	using Estimator=Nop::Estimator;

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator; 
};

std::ostream& operator<<(std::ostream&,Shooter_feed::Goal);
std::ostream& operator<<(std::ostream&,Shooter_feed);

bool operator==(Shooter_feed::Output_applicator,Shooter_feed::Output_applicator);

bool operator==(Shooter_feed,Shooter_feed);
bool operator!=(Shooter_feed,Shooter_feed);

std::set<Shooter_feed::Goal> examples(Shooter_feed::Goal*);
std::set<Shooter_feed::Status_detail> examples(Shooter_feed::Status_detail*);

Shooter_feed::Output control(Shooter_feed::Status_detail, Shooter_feed::Goal);
Shooter_feed::Status status(Shooter_feed::Status_detail);
bool ready(Shooter_feed::Status, Shooter_feed::Goal);

#endif
