#ifndef DRIVEBASE_H
#define DRIVEBASE_H

#include <iosfwd>
#include <set>
#include "../util/interface.h"
#include "motor_check.h"
#include "../util/quick.h"
#include "../util/countdown_timer.h"
#include "../util/stall_monitor.h"

struct Drivebase{
	enum Motor{LEFT1,LEFT2,LEFT3,RIGHT1,RIGHT2,RIGHT3,MOTORS};

	typedef std::pair<Digital_in,Digital_in> Encoder_info;
	
	#define ENCODER_TICKS(X) \
		X(int,l) \
		X(int,r)
	DECLARE_STRUCT(Encoder_ticks,ENCODER_TICKS)

	#define DISTANCES_ITEMS(X) \
		X(double,l) \
		X(double,r)
	DECLARE_STRUCT(Distances,DISTANCES_ITEMS)

	#define DRIVEBASE_INPUT(X) \
		X(SINGLE_ARG(std::array<double,MOTORS>),current) \
		X(Encoder_info,left) \
		X(Encoder_info,right) \
		X(Distances,distances) 
	DECLARE_STRUCT(Input,DRIVEBASE_INPUT)

	struct Input_reader{
		Input operator()(Robot_inputs const&)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};
	Input_reader input_reader;

	#define DRIVEBASE_OUTPUT(X)\
		X(double,l)\
		X(double,r)
	DECLARE_STRUCT(Output,DRIVEBASE_OUTPUT)

	#define SPEEDS_ITEMS(X) \
		X(double,l) \
		X(double,r) 
	DECLARE_STRUCT(Speeds,SPEEDS_ITEMS) //consider renaming to Velocities

	#define DRIVEBASE_STATUS(X) \
		X(SINGLE_ARG(std::array<Motor_check::Status,MOTORS>),motor)\
		X(bool,stall) \
		X(Speeds,speeds)\
		X(Distances,distances) 
	DECLARE_STRUCT(Status,DRIVEBASE_STATUS)

	typedef Status Status_detail;

	struct Estimator{
		std::array<Motor_check,MOTORS> motor_check;
		Status_detail last;
		Countdown_timer speed_timer;
		Stall_monitor stall_monitor;

		void update(Time,Input,Output);
		Status_detail get()const;
		Estimator();
	};
	Estimator estimator;

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output)const;
		Output operator()(Robot_outputs)const;
	};
	Output_applicator output_applicator;

	struct Goal{
		#define DRIVEBASE_GOAL_MODES \
			X(ABSOLUTE) \
			X(DISTANCE)
		#define X(name) name,
		enum class Mode{DRIVEBASE_GOAL_MODES};
		#undef X
		Mode mode;

		double distance;
		double left,right;
	};
};
bool operator==(Drivebase::Encoder_ticks const&,Drivebase::Encoder_ticks const&);
bool operator!=(Drivebase::Encoder_ticks const&,Drivebase::Encoder_ticks const&);
bool operator<(Drivebase::Encoder_ticks const&,Drivebase::Encoder_ticks const&);
Drivebase::Encoder_ticks operator+(Drivebase::Encoder_ticks const&,Drivebase::Encoder_ticks const&);
Drivebase::Encoder_ticks operator-(Drivebase::Encoder_ticks const&);
Drivebase::Encoder_ticks operator-(Drivebase::Encoder_ticks const&,Drivebase::Encoder_ticks const&);
std::ostream& operator<<(std::ostream&,Drivebase::Encoder_ticks const&);

bool operator==(Drivebase::Distances const&,Drivebase::Distances const&);
bool operator!=(Drivebase::Distances const&,Drivebase::Distances const&);
bool operator<(Drivebase::Distances const&,Drivebase::Distances const&);
std::ostream& operator<<(std::ostream&,Drivebase::Distances const&);
Drivebase::Distances operator+(Drivebase::Distances const&,Drivebase::Distances const&);
Drivebase::Distances& operator+=(Drivebase::Distances&,Drivebase::Distances const&);
Drivebase::Distances operator-(Drivebase::Distances const&);
Drivebase::Distances operator-(Drivebase::Distances const&,Drivebase::Distances const&);
Drivebase::Distances fabs(Drivebase::Distances const&);

double ticks_to_inches(const int);
int inches_to_ticks(const double);
Drivebase::Distances ticks_to_inches(const Drivebase::Encoder_ticks);
Drivebase::Encoder_ticks inches_to_ticks(const Drivebase::Distances);

int encoderconv(Maybe_inline<Encoder_output>);

CMP1(Drivebase::Encoder_ticks)
CMP1(Drivebase::Speeds)

std::ostream& operator<<(std::ostream&,Drivebase::Input const&);
bool operator<(Drivebase::Input const&,Drivebase::Input const&);
bool operator==(Drivebase::Input const&,Drivebase::Input const&);
bool operator!=(Drivebase::Input const&,Drivebase::Input const&);
std::set<Drivebase::Input> examples(Drivebase::Input*);

std::ostream& operator<<(std::ostream&,Drivebase::Output const&);
bool operator<(Drivebase::Output const&,Drivebase::Output const&);
bool operator==(Drivebase::Output const&,Drivebase::Output const&);
bool operator!=(Drivebase::Output const&,Drivebase::Output const&);
std::set<Drivebase::Output> examples(Drivebase::Output*);

CMP1(Drivebase::Status)
std::set<Drivebase::Status> examples(Drivebase::Status*);

std::ostream& operator<<(std::ostream&,Drivebase::Goal const&);
std::set<Drivebase::Goal> examples(Drivebase::Goal*);
bool operator<(Drivebase::Goal const&,Drivebase::Goal const&);

Drivebase::Status status(Drivebase::Status_detail);
Drivebase::Output control(Drivebase::Status_detail,Drivebase::Goal);
bool ready(Drivebase::Status,Drivebase::Goal);

bool operator==(Drivebase::Estimator const&,Drivebase::Estimator const&);
bool operator!=(Drivebase::Estimator const&,Drivebase::Estimator const&);

bool operator!=(Drivebase const&,Drivebase const&);
std::ostream& operator<<(std::ostream&,Drivebase const&);

float range(const Robot_inputs); //To be used for wall following NOT DONE 

#endif
