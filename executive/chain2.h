#ifndef CHAIN2_H
#define CHAIN2_H

#include "executive.h"
#include "step.h"

struct Chain2:Executive_impl<Chain2>{
	//enum class Failure_procedure{TREAT_AS_SUCCESS,ENTER_TELEOP,ABORT};
	unsigned current_step;
	std::vector<Step> steps;
	Executive next;

	Chain2(std::vector<Step>, Executive);
	Chain2(Step, Executive);

	Toplevel::Goal run(Run_info);
	Executive next_mode(Next_mode_info);
	bool operator==(Chain2 const&)const;
	std::unique_ptr<Executive_interface> clone()const;
	void display(std::ostream&)const;
};

#endif
