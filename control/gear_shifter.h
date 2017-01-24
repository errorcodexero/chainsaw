#ifndef GEAR_SHIFTER
#define GEAR_SHIFTER

#include "../util/countdown_timer.h"
#include "nop.h"
#include "drivebase.h"

struct Gear_shifter{
	/*struct Input{
		double current;//total, in amps
		double speed; //in ft/s
	};

	struct Input_reader{
		Input operator()(Robot_inputs const&)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};*/
	using Input_reader=Drivebase::Input_reader;
	using Input=Drivebase::Input;
	Input_reader input_reader;

	#define GEAR_SHIFTER_OUTPUTS(X) X(HIGH) X(LOW)
	enum class Output {
		#define X(A) A,
		GEAR_SHIFTER_OUTPUTS(X)
		#undef X
	};

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output)const;
		Output operator()(Robot_outputs)const;
	};
	Output_applicator output_applicator;

	#define GEAR_SHIFTER_GOALS(X) X(AUTO) X(HIGH) X(LOW)
	enum class Goal{
		#define X(A) A,
		GEAR_SHIFTER_GOALS(X)
		#undef X
	};

	using Status=Nop::Status;

	using Status_detail=Output;//what auto-shifting code thinks should happen.

	struct Estimator{
		class Speed_tracker{
			Time last_time;
			int last_ticks;

			public:
			Speed_tracker();
			double update(Time,int ticks);
		};
		Speed_tracker speed_tracker_l,speed_tracker_r;

		double last_current;

		Countdown_timer no_shift;
		Output last_output;
		Output recommended;

		Estimator();
		Status_detail get()const;
		void update(Time,Input,Output);
	};
	Estimator estimator;
};

std::ostream& operator<<(std::ostream&,Gear_shifter const&);
//std::ostream& operator<<(std::ostream&,Gear_shifter::Input);
std::ostream& operator<<(std::ostream&,Gear_shifter::Output);
std::ostream& operator<<(std::ostream&,Gear_shifter::Goal);

std::set<bool> examples(const bool*);
std::set<Gear_shifter::Goal> examples(const Gear_shifter::Goal*);
std::set<Gear_shifter::Output> examples(const Gear_shifter::Output*);
//std::set<Gear_shifter::Input> examples(const Gear_shifter::Input*);

/*bool operator<(Gear_shifter::Input,Gear_shifter::Input);
bool operator==(Gear_shifter::Input,Gear_shifter::Input);
bool operator!=(Gear_shifter::Input,Gear_shifter::Input);*/

bool ready(Gear_shifter::Status,Gear_shifter::Goal);
Gear_shifter::Output control(Gear_shifter::Status_detail,Gear_shifter::Goal);
Gear_shifter::Status status(Gear_shifter::Status_detail);

#endif
