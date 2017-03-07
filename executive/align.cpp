#include "align.h"
#include <iostream>

using namespace std;

ostream& operator<<(ostream& o,Align::Mode a){
	switch(a){
		case Align::Mode::VISION:
			return o << "VISION";
		case Align::Mode::NONVISION:    
			return o << "NON-VISION";
		default:
			assert(0);
	}
}

static const int CENTER = 115;
static const Time INITIAL_SEARCH = 0.1;

Align::Align(Rad a):mode(Align::Mode::VISION),blocks({}),current(0),center(CENTER),estimated_angle(a),nonvision_align(Step{Turn(a)}){
	initial_search.set(INITIAL_SEARCH);
}
Align::Align():Align(0){}

void Align::update(Camera camera){
	if((!camera.enabled || camera.blocks.empty()) && initial_search.done()){
		//TODO: should search for vision target instead
		mode = Mode::NONVISION;
	} else {
		mode = Mode::VISION;
		blocks = camera.blocks;
		//TODO: check for two largest blocks that are in the expected y range
		if(blocks.size()>=2) current = mean(blocks[0].x,blocks[1].x);
		else if(blocks.size()==1) current = blocks[0].x;
		//if we dont see any blobs we will continue as if we still see what we saw before there being no blocks
	}
}

Step::Status Align::done(Next_mode_info info){
	update(info.in.camera);
	switch(mode){
		case Mode::VISION:
			{
				if(info.since_switch>10){ //time is seince begining of auto period
					return Step::Status::FINISHED_SUCCESS;
				}
				const int TOLERANCE = 2;
				if(current > center - TOLERANCE && current < center + TOLERANCE){
					in_range.update(info.in.now,info.in.robot_mode.enabled);
				} else {
					static const Time FINISH_TIME = 1.0;
					in_range.set(FINISH_TIME);
				}
				return in_range.done() ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
			}
		case Mode::NONVISION:
			return nonvision_align.done(info);
		default:
			assert(0);
	}
}

Toplevel::Goal Align::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Align::run(Run_info info,Toplevel::Goal goals){
	initial_search.update(info.in.now,info.in.robot_mode.enabled);
	goals.lights.camera_light=1;
	update(info.in.camera);
	cout<<"Align:    mode:"<<mode<<" blocks:"<<blocks<<"   current:"<<current<<"   center:"<<center<<"    angle:"<<estimated_angle<<"\n";
	goals.shifter = Gear_shifter::Goal::LOW;
	switch(mode){
		case Mode::VISION:
			{
				/*double power = target_to_out_power([&]{//power for left side, right side is opposite of this
					const double SMALL_POWER = .1, LARGE_POWER = .15;//TODO: make it slow as it appraches the center?
					//if it needs to turn more, turn faster
					const int FINE_TOLERANCE = 4;
					
					
					const int COARSE_TOLERANCE = FINE_TOLERANCE * 2;
					
					if(current < center - COARSE_TOLERANCE){
						return -LARGE_POWER;
					}
					if(current > center + COARSE_TOLERANCE){
						return LARGE_POWER;
					} 
					
					//if it needs to turn less, turn less
					if(current < center - FINE_TOLERANCE){
						return -SMALL_POWER;
					}
					if(current > center + FINE_TOLERANCE){
						return SMALL_POWER;
					} 
					return 0.0;
				}());
				goals.drive.left = power;
				goals.drive.right = -power;
				*/
				const double power = .15;//TODO: make a motion profile somehow?
				const int TOLERANCE = 2;
				if(current < center - TOLERANCE){
					goals.drive.left = -power;
					goals.drive.right = power;
					} else if(current > center + TOLERANCE){
					goals.drive.left = power;
					goals.drive.right = -power;
				} else {
					goals.drive.left = 0;
					goals.drive.right = 0;
				}
				return goals;
			}
		case Mode::NONVISION:
			return nonvision_align.run(info,goals);
		default:
			assert(0);
	}
}

unique_ptr<Step_impl> Align::clone()const{
	return unique_ptr<Step_impl>(new Align(*this));
}

bool Align::operator==(Align const& b)const{
	return mode == b.mode && blocks == b.blocks && current == b.current && center == b.center && in_range == b.in_range && estimated_angle == b.estimated_angle && initial_search == b.initial_search && nonvision_align == b.nonvision_align;
}

