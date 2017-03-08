#ifndef ALIGN_H
#define ALIGN_H

#include "executive.h"
#include "../util/motion_profile.h"
#include "step.h"

struct Align: public Step_impl_inner<Align>{
	enum class Mode{VISION,NONVISION};
	Mode mode;
	std::vector<Pixy::Block> blocks;
	int current;
	Countdown_timer in_range;	
	Countdown_timer initial_search;
	
	Rad estimated_angle;
	Step nonvision_align;
	
	void update(Camera);

	public:
	explicit Align(Rad); //TODO: pass in initial encoders or find some way to set them inside of the Turn step
	explicit Align();

	Toplevel::Goal run(Run_info,Toplevel::Goal);
	Toplevel::Goal run(Run_info);
	Step::Status done(Next_mode_info);
	std::unique_ptr<Step_impl> clone()const;
	bool operator==(Align const&)const;
};
#endif
