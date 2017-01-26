#ifndef GEAR_GRABBER_H
#define GEAR_GRABBER_H

#include "../util/interface.h"
#include <set>
#include "../util/countdown_timer.h"

struct Gear_grabber{
	enum class Goal{OPEN,CLOSE};
	
	typedef Goal Output;

	struct Input{
		bool closed;//hall-effect
		bool enabled;
		Input();
		Input(bool,bool);
	};
	
	struct Input_reader{
		Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};

	enum class Status_detail{OPEN,OPENING,CLOSING,CLOSED};

	typedef Status_detail Status;	
	
	struct Output_applicator{
		Output operator()(Robot_outputs)const;
		Robot_outputs operator()(Robot_outputs,Output)const;
	};

	struct Estimator{
		Status_detail last;	
		Countdown_timer open_timer;
		Countdown_timer close_timer;
	
		void update(Time,Input,Output);
		Status_detail get()const;
	
		Estimator();
	};
	
	Input_reader input_reader;
	Output_applicator output_applicator;
	Estimator estimator;
};


std::set<Gear_grabber::Goal> examples(Gear_grabber::Goal*);
std::set<Gear_grabber::Input> examples(Gear_grabber::Input*);
std::set<Gear_grabber::Status_detail> examples(Gear_grabber::Status_detail*);

std::ostream& operator<<(std::ostream&,Gear_grabber);
std::ostream& operator<<(std::ostream&,const Gear_grabber::Input);
std::ostream& operator<<(std::ostream&,const Gear_grabber::Goal);
std::ostream& operator<<(std::ostream&,const Gear_grabber::Status_detail);

bool operator==(const Gear_grabber::Input,const Gear_grabber::Input);
bool operator!=(const Gear_grabber::Input,const Gear_grabber::Input);
bool operator<(const Gear_grabber::Input,const Gear_grabber::Input);

bool operator==(const Gear_grabber::Estimator,const Gear_grabber::Estimator);
bool operator!=(const Gear_grabber::Estimator,const Gear_grabber::Estimator);

Gear_grabber::Output control(Gear_grabber::Status,Gear_grabber::Goal);
Gear_grabber::Status status(Gear_grabber::Status_detail);
bool ready(Gear_grabber::Status,Gear_grabber::Goal);

#endif
