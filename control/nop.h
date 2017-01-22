#ifndef NOP_H
#define NOP_H

#include "../util/interface.h"

struct Nop{
	struct Input{};
	using Output=Input;
	using Status_detail=Input;
	using Status=Status_detail;
	using Goal=Input;

	struct Input_reader{
		Input operator()(Robot_inputs const&)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};

	struct Output_applicator{
		Output operator()(Robot_outputs)const;
		Robot_outputs operator()(Robot_outputs,Output)const;
	};

	template<typename Status_detail>
	struct Estimator_nop{
		template<typename Input,typename Output>
		void update(Time,Input,Output){}

		Status_detail get()const{ return {}; }
	};
	using Estimator=Estimator_nop<Status_detail>;

	Input_reader input_reader;
	Output_applicator output_applicator;
	Estimator estimator;
};

bool operator<(Nop::Input,Nop::Input);
bool operator==(Nop::Input,Nop::Input);
bool operator!=(Nop::Input,Nop::Input);
std::set<Nop::Input> examples(Nop::Input*);
std::ostream& operator<<(std::ostream&,Nop::Input const&);

bool operator==(Nop::Input_reader,Nop::Input_reader);
bool operator==(Nop::Output_applicator,Nop::Output_applicator);

template<typename T>
bool operator==(Nop::Estimator_nop<T>,Nop::Estimator_nop<T>){
	return 1;
}

bool operator!=(Nop::Estimator,Nop::Estimator);

std::ostream& operator<<(std::ostream&,Nop const&);

Nop::Output control(Nop::Status_detail,Nop::Goal);
Nop::Status status(Nop::Status_detail);
bool ready(Nop::Status,Nop::Goal);

#endif
