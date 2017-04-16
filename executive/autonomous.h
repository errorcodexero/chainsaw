#ifndef AUTONOMOUS_H
#define AUTONOMOUS_H

#include "executive.h"
#include "../util/auto_distances.h"

struct Autonomous: public Executive_impl<Autonomous>{
	Auto_distances auto_distances;

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Autonomous const&)const;

	Autonomous();
	Autonomous(Auto_distances);
};

#endif
