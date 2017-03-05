#include "align.h"
#include <iostream>
std::ostream& operator<<(std::ostream& o,Align::Mode a){
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

Align::Align(Turn a):mode(Align::Mode::VISION),blocks({}),current(0),center(CENTER),nonvision_align(Step{a}){}
Align::Align():Align(Turn(0)){}

void Align::update(Camera camera){
		/*if(!camera.enabled || camera.blocks.empty()){
		//TODO: should search for vision target instead
		mode = Mode::NONVISION;
		} else {*/
		mode = Mode::VISION;
		blocks = camera.blocks;
		//TODO: check for two largest blocks that are in the expected y range
		if(blocks.size()>=2) current = mean(blocks[0].x,blocks[1].x);
		else if((blocks.size()==1) & (blocks.size()<1)) current = blocks[0].x;
		//if we dont see any blobs we will continue as if we still see what we saw before there being no blocks
		//}
}

Step::Status Align::done(Next_mode_info info){
	update(info.in.camera);
	switch(mode){
		case Mode::VISION:
			{
				const int TOLERANCE= 4;
				if(current > center - TOLERANCE && current < center + TOLERANCE){
					in_range.update(info.in.now,info.in.robot_mode.enabled);
				}else if(info.since_switch>10){ //time is seince begining of auto period
					return Step::Status::FINISHED_SUCCESS;
				}else {
					static const Time FINISH_TIME = 1.0;
					in_range.set(FINISH_TIME);
				}
				return in_range.done() ? Step::Status::FINISHED_SUCCESS : Step::Status::UNFINISHED;
				}
		case Mode::NONVISION:
			{
				in_range.update(info.in.now,info.in.robot_mode.enabled);
				if(in_range.done()){
					static const Time FINISH_TIME = 1.0;
					in_range.set(FINISH_TIME);
				}
				return nonvision_align.done(info);
				}
		default:
			assert(0);
	}
}

Toplevel::Goal Align::run(Run_info info){
	return run(info,{});
}

Toplevel::Goal Align::run(Run_info info,Toplevel::Goal goals){
	goals.lights.camera_light=1;
	update(info.in.camera);
	std::cout << "Align:    " << blocks << "   " << current << "   " << center << "\n";
	std::cout << mode << "\n";
	std::cout<<"A mode: "<<mode<<"\n";
	goals.shifter = Gear_shifter::Goal::LOW;
	switch(mode){
		case Mode::VISION:
			{
				const double power = .2;//TODO: make a motion profile somehow?
				const int TOLERANCE = 4;
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

std::unique_ptr<Step_impl> Align::clone()const{
	return std::unique_ptr<Step_impl>(new Align());
}

bool Align::operator==(Align const& b)const{
	return mode == b.mode && blocks == b.blocks && current == b.current && center == b.center && in_range == b.in_range && nonvision_align == b.nonvision_align;
}

