#include "teleop.h"
#include <math.h>
#include "autonomous.h"
#include "../input/util.h"

using namespace std;

#define CLEAR_BALL_TIME 2 //seconds

double set_drive_speed(double axis,double boost,bool slow){
	static const float MAX_SPEED=1;//Change this value to change the max power the robot will achieve with full boost (cannot be larger than 1.0)
	static const float DEFAULT_SPEED=.4;//Change this value to change the default power
	static const float SLOW_BY=.5;//Change this value to change the percentage of the default power the slow button slows
	return (pow(axis,3)*((DEFAULT_SPEED+(MAX_SPEED-DEFAULT_SPEED)*boost)-((DEFAULT_SPEED*SLOW_BY)*slow)));
}

bool operator<(Teleop::Nudge const& a,Teleop::Nudge const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	NUDGE_ITEMS(X)
	#undef X
	return 0;
}

bool operator==(Teleop::Nudge const& a,Teleop::Nudge const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	NUDGE_ITEMS(X)
	#undef X
	return 1;
}

ostream& operator<<(ostream& o,Teleop::Nudge const& a){
	#define X(t,NAME) o<<""#NAME<<":"<<(a.NAME)<<" ";
	NUDGE_ITEMS(X)
	#undef X
	return o;
}

ostream& operator<<(ostream& o,Teleop::Gear_collector_mode const& a){
	#define X(NAME) if(a==Teleop::Gear_collector_mode::NAME) return o<<""#NAME;
	GEAR_COLLECTOR_MODES
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Teleop::Collect_step const& a){
	#define X(NAME) if(a==Teleop::Collect_step::NAME) return o<<""#NAME;
	COLLECT_STEPS
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Teleop::No_collect_step const& a){
	#define X(NAME) if(a==Teleop::No_collect_step::NAME) return o<<""#NAME;
	NO_COLLECT_STEPS
	#undef X
	assert(0);
}

Executive Teleop::next_mode(Next_mode_info info) {
	if (info.autonomous_start) {
		if (info.panel.in_use) {
			return Executive{Autonomous()};
		}
	}
	//Teleop t(CONSTRUCT_STRUCT_PARAMS(TELEOP_ITEMS));
	return Executive{*this};
}

IMPL_STRUCT(Teleop::Teleop,TELEOP_ITEMS)

Teleop::Teleop():gear_collector_mode(Gear_collector_mode::COLLECT),collect_step(Collect_step::GEAR_COLLECTOR_DOWN),no_collect_step(No_collect_step::BALL_COLLECTOR_IN),print_number(0){}

void Teleop::collect_protocol(Toplevel::Status_detail const& status,Toplevel::Goal& goals){
	switch(collect_step){
		case Collect_step::GEAR_COLLECTOR_DOWN:
			goals.gear_collector=Gear_collector::Goal{Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::DOWN,Roller::Goal::OFF,Roller_arm::Goal::STOW};
			if(status.gear_collector.gear_lifter==Gear_lifter::Status_detail::DOWN) collect_step = Collect_step::COLLECT; //TODO: replace with ready?
			break;	
		case Collect_step::COLLECT:
			goals.gear_collector=Gear_collector::Goal{Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::DOWN,Roller::Goal::OFF,Roller_arm::Goal::STOW};
			break;
		default:
			assert(0);
	}	
}

void Teleop::no_collect_protocol(Toplevel::Status_detail const& /*status*/,const bool enabled,const Time now,Toplevel::Goal& goals){
	switch(no_collect_step){
		case No_collect_step::BALL_COLLECTOR_IN:
			goals.gear_collector=Gear_collector::Goal{Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::DOWN,Roller::Goal::OFF,Roller_arm::Goal::STOW};

			no_collect_step=No_collect_step::CLEAR_BALLS;
			clear_ball_timer.set(CLEAR_BALL_TIME);
			break;
		case No_collect_step::CLEAR_BALLS:
			clear_ball_timer.update(now,enabled);
			goals.gear_collector=Gear_collector::Goal{Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::DOWN,Roller::Goal::OFF,Roller_arm::Goal::STOW};
			if(clear_ball_timer.done()) no_collect_step=No_collect_step::GEAR_COLLECTOR_ACTIVE;
			break;
		case No_collect_step::GEAR_COLLECTOR_ACTIVE:
			goals.gear_collector=[&]{
				switch(gear_collector_mode){
					case Gear_collector_mode::PREP_COLLECT: return Gear_collector::Goal{Gear_grabber::Goal::OPEN,Gear_lifter::Goal::DOWN,Roller::Goal::OFF,Roller_arm::Goal::STOW};
					case Gear_collector_mode::COLLECT: return Gear_collector::Goal{Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::DOWN,Roller::Goal::OFF,Roller_arm::Goal::STOW};
					case Gear_collector_mode::PREP_SCORE: return Gear_collector::Goal{Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::UP,Roller::Goal::OFF,Roller_arm::Goal::STOW};
					case Gear_collector_mode::SCORE: return Gear_collector::Goal{Gear_grabber::Goal::OPEN,Gear_lifter::Goal::UP,Roller::Goal::OFF,Roller_arm::Goal::STOW};
					default: assert(0);
				}
			}();
			break;
		default:
			assert(0);
	}	
}

Toplevel::Goal Teleop::run(Run_info info) {
	Toplevel::Goal goals;
	
	bool enabled = info.in.robot_mode.enabled;

	{//Set drive goals
		bool spin=fabs(info.driver_joystick.axis[Gamepad_axis::RIGHTX])>.01;//drive turning button
		double boost=info.driver_joystick.axis[Gamepad_axis::LTRIGGER]; //Turbo button
		bool slow=info.driver_joystick.button[Gamepad_button::LB]; //Slow button

		POV_section driver_pov=pov_section(info.driver_joystick.pov);
		
		const array<POV_section,NUDGES> nudge_povs={POV_section::UP,POV_section::DOWN,POV_section::RIGHT,POV_section::LEFT};
		//Forward, backward, clockwise, counter-clockwise
		for(unsigned i=0;i<NUDGES;i++){
			if(nudges[i].trigger(boost<.25 && driver_pov==nudge_povs[i])) nudges[i].timer.set(.1);
			nudges[i].timer.update(info.in.now,enabled);
		}
		const double NUDGE_POWER=.2,ROTATE_NUDGE_POWER=.2;
		goals.drive.left=([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return NUDGE_POWER;
			if(!nudges[Nudges::BACKWARD].timer.done()) return -NUDGE_POWER;
			if(!nudges[Nudges::CLOCKWISE].timer.done()) return ROTATE_NUDGE_POWER;
			if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return -ROTATE_NUDGE_POWER;
			double power=set_drive_speed(info.driver_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power+=set_drive_speed(-info.driver_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return -power; //inverted so drivebase values can be positive
		}());
		goals.drive.right=clip([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return NUDGE_POWER;
			if(!nudges[Nudges::BACKWARD].timer.done()) return -NUDGE_POWER;
			if(!nudges[Nudges::CLOCKWISE].timer.done()) return -ROTATE_NUDGE_POWER;	
			if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return ROTATE_NUDGE_POWER;
			double power=set_drive_speed(info.driver_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power-=set_drive_speed(-info.driver_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return -power; //inverted so drivebase values can be positive
		}());
	}

	goals.shifter=[&]{
		if(info.driver_joystick.button[Gamepad_button::RB]) return Gear_shifter::Goal::LOW;
		if(info.driver_joystick.axis[Gamepad_axis::RTRIGGER]>.8) return Gear_shifter::Goal::HIGH;
		return Gear_shifter::Goal::AUTO;
	}();

	if(info.panel.gear_prep_collect) gear_collector_mode=Gear_collector_mode::PREP_COLLECT;
	if(info.panel.gear_collect) gear_collector_mode=Gear_collector_mode::COLLECT;
	if(info.panel.gear_prep_score) gear_collector_mode=Gear_collector_mode::PREP_SCORE;
	if(info.panel.gear_score) gear_collector_mode=Gear_collector_mode::SCORE;

	collect.update(info.panel.ball_collect);
	/*bool collect_t=collect_trigger(info.panel.ball_collect);
	if(collect.get()) {
		if(collect_t) collect_step=Collect_step::GEAR_COLLECTOR_DOWN;
		collect_protocol(info.status,goals);
	} else {
		if(collect_t) no_collect_step=No_collect_step::BALL_COLLECTOR_IN;
		no_collect_protocol(info.status,enabled,info.in.now,goals);
	}*/

	if (collect.get()){
	} else {
		goals.gear_collector=[&]{
			switch(gear_collector_mode){
				case Gear_collector_mode::PREP_COLLECT: 
					return Gear_collector::Goal{Gear_grabber::Goal::OPEN,Gear_lifter::Goal::DOWN,Roller::Goal::OFF,Roller_arm::Goal::STOW};
				case Gear_collector_mode::COLLECT: return Gear_collector::Goal{Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::DOWN,Roller::Goal::OFF,Roller_arm::Goal::STOW};
				case Gear_collector_mode::PREP_SCORE: return Gear_collector::Goal{Gear_grabber::Goal::CLOSE,Gear_lifter::Goal::UP,Roller::Goal::OFF,Roller_arm::Goal::STOW};
				case Gear_collector_mode::SCORE: return Gear_collector::Goal{Gear_grabber::Goal::OPEN,Gear_lifter::Goal::UP,Roller::Goal::OFF,Roller_arm::Goal::STOW};
				default: assert(0);
			}
		}();
	}

	goals.climber=info.panel.climb?Climber::Goal::CLIMB:Climber::Goal::STOP;

	indicator_toggle.update(info.panel.loading_indicator);
	if(indicator_toggle.get()) goals.lights.loading_indicator=Lights::Loading_indicator::BALLS;
	else goals.lights.loading_indicator=Lights::Loading_indicator::GEARS;

	camera_light_toggle.update(info.driver_joystick.button[Gamepad_button::START]);
	goals.lights.camera_light=camera_light_toggle.get();//TODO

	//Manual controls
	if(info.panel.gear_grabber==Panel::Gear_grabber::OPEN) goals.gear_collector.gear_grabber=Gear_grabber::Goal::OPEN;
	if(info.panel.gear_grabber==Panel::Gear_grabber::CLOSED) goals.gear_collector.gear_grabber=Gear_grabber::Goal::CLOSE;
	if(info.panel.gear_arm==Panel::Gear_arm::UP) goals.gear_collector.gear_lifter=Gear_lifter::Goal::UP;
	if(info.panel.gear_arm==Panel::Gear_arm::DOWN) goals.gear_collector.gear_lifter=Gear_lifter::Goal::DOWN;	

	goals.shooter = [&]{
		switch(info.panel.shooter){
			case Panel::Shooter::ENABLED: return Shooter::Goal::FORWARD;
			case Panel::Shooter::AUTO: 
				if(info.panel.shoot) return Shooter::Goal::FORWARD;
				return Shooter::Goal::OFF;
			case Panel::Shooter::DISABLED: return Shooter::Goal::OFF;
			default:
				nyi
		}
	}();

	#if 1
	if(info.in.ds_info.connected && (print_number%10)==0){
		cout<<"\nstalled:"<<info.status.drive.stall<<"\n";
		if(info.in.camera.enabled){
			cout<<"size: "<<info.in.camera.blocks.size()<<" blocks: "<<info.in.camera.blocks<<"\n";
			/*for (vector<Pixy::Block>::const_iterator it=info.in.camera.blocks.begin();it!=info.in.camera.blocks.end();it++){
				cout<<"\tarea: "<<(it->width * it->height)<<"\n";
			}*/
		}
		cout<<"\n";
	}
	#endif
	print_number++;
	
	return goals;
}

bool Teleop::operator<(Teleop const& a)const{
	#define X(t,NAME) if(NAME<a.NAME) return 1; if(a.NAME<NAME) return 0;
	TELEOP_ITEMS(X)
	#undef X
	return 0;
}

bool Teleop::operator==(Teleop const& a)const{
	#define X(t,NAME) if(NAME!=a.NAME) return 0;
	TELEOP_ITEMS(X)
	#undef X
	return 1;
}

void Teleop::display(ostream& o)const{
	o<<"Teleop( ";
	#define X(t,NAME) o<<""#NAME<<":"<<(NAME)<<" ";
	TELEOP_ITEMS(X)
	#undef X
	o<<")";
}

#ifdef TELEOP_TEST
#include "test.h"

int main() {
	Teleop a;
	test_executive(a);
}
#endif
