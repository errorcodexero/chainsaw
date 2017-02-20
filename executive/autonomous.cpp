#include "autonomous.h"

#include "teleop.h" 
#include "chain.h"
#include "step.h"

using namespace std; 
using Mode=Executive;

double deg_to_rad(double deg){
	return deg/180*PI;
}

//using Geardrop=Drive_straight{0}; //TODO: Fixme
auto Geardrop(){ return Drive_straight{0}; }

Executive auto_mode_convert(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	cout<<"panel in use "<<info.panel.in_use<<" auto panel value "<<info.panel.auto_select<<"\n";

	//for when just want to run across the field at the end of autonomous
	Executive dash{Chain{
		Step{Drive_straight{12*20}},
		Executive{Teleop{}}
	}};

	Executive backoff{Chain{
		Step{Drive_straight{-12}},
		Executive{Teleop{}}
	}};

	Executive auto_null{Teleop{}};

	Executive auto_baseline{Chain{
		Step{Drive_straight{12*12}},
		Executive{Teleop{}}
	}};
	
	Executive drive_straight_test{Chain{
		Step{Drive_straight{3*12}},
		Executive{Teleop{}}
	}};

	Executive turn_test{Chain{
		Step{Turn{PI/2}},
		Executive{Teleop{}}
	}};

	if (info.panel.in_use) {
		switch(info.panel.auto_select){ 
			case 0: //Do Nothing
				return auto_null;
			case 1: //Baseline
				return turn_test;
				//return drive_straight_test; 
				//return auto_baseline;//TODO
			case 2: //Baseline Extended
				return Executive{Chain{
					Step{Drive_straight{12*12}},
					dash
				}};
			case 3: //Gear Boiler
				return Executive{Chain{
					Step{Drive_straight{5*12}},
					Executive{Chain{
						Step{Turn{deg_to_rad(40)}},
						Executive{Chain{
							Step{Drive_straight{12}}, //approach
							Executive{Chain{
								Step{Geardrop()},
								backoff
							}}
						}}
					}}
				}};
			case 4: //Gear Boiler Extended
				return Executive{Chain{
					Step{Drive_straight{5*12}},
					Executive{Chain{
						Step{Turn{deg_to_rad(40)}},
						Executive{Chain{
							Step{Drive_straight{12}}, //approach
							Executive{Chain{
								Step{Geardrop()},
								Executive{Chain{
									Step{Drive_straight{-12}},
									Executive{Chain{
										Step{Turn{deg_to_rad(-40)}},
										dash
									}}
								}}
							}}
						}}
					}}
				}};
			case 5: //Gear Loading
				return Executive{Chain{
					Step{Drive_straight{5*12}},
					Executive{Chain{
						Step{Turn{deg_to_rad(-40)}},
						Executive{Chain{
							Step{Drive_straight{12}}, //approach
							Executive{Chain{
								Step{Geardrop()},
								backoff
							}}
						}}
					}}
				}};
			case 6: //Gear Loading Extended
				return Executive{Chain{
					Step{Drive_straight{5*12}},
					Executive{Chain{
						Step{Turn{deg_to_rad(-40)}},
						Executive{Chain{
							Step{Drive_straight{12}}, //approach
							Executive{Chain{
								Step{Geardrop()},
								Executive{Chain{
									Step{Drive_straight{-12}},
									Executive{Chain{
										Step{Turn{deg_to_rad(40)}},
										dash
									}}
								}}
							}}
						}}
					}}
				}};	
			case 7: //Gear Mid
				return Executive{Chain{
					Step{Drive_straight{10*12}},
					Executive{Chain{
						Step{Geardrop()},
						backoff
					}}
				}};
			case 8: //Gear mid Extended right
				return Executive{Chain{
					Step{Drive_straight{10*12}},
					Executive{Chain{
						Step{Geardrop()},
						Executive{Chain{
							Step{Drive_straight{-12}},
							Executive{Chain{
								Step{Turn{deg_to_rad(-45)}},
								Executive{Chain{
									Step{Drive_straight{3*12}},
									Executive{Chain{
										Step{Turn{deg_to_rad(45)}},
										dash
									}}
								}}
							}}
						}}
					}}
				}};
			case 9: //Gear mid Extended left
				return Executive{Chain{
					Step{Drive_straight{10*12}},
					Executive{Chain{
						Step{Geardrop()},
						Executive{Chain{
							Step{Drive_straight{-12}},
							Executive{Chain{
								Step{Turn{deg_to_rad(45)}},
								Executive{Chain{
									Step{Drive_straight{3*12}},
									Executive{Chain{
										Step{Turn{deg_to_rad(-45)}},
										dash
									}}
								}}
							}}
						}}
					}}
				}};
			case 10: //Vision test (TEMP)
				return Executive{Chain{
					Step{Drive_straight{5*12}},
					Executive{Chain{
						Step{Turn{deg_to_rad(40)}},
						Executive{Chain{
							Step{Drive_straight{12}}, //approach
							Executive{Chain{
								Step{Geardrop()},
								backoff
							}}
						}}
					}}
				}};
			default:
				return auto_null;
		}
	}
	return auto_baseline;//default executive is no panel exists. Noramlly Teleop
}

Mode Autonomous::next_mode(Next_mode_info info){
	static const Time DELAY = 0.0;//seconds, TODO: base it off of the dial on the OI?
	if(!info.autonomous) return Mode{Teleop()};
	if(info.since_switch > DELAY) return auto_mode_convert(info);
	return Mode{Autonomous()};
}

Toplevel::Goal Autonomous::run(Run_info){
	
	return {};
}

bool Autonomous::operator==(Autonomous const&)const{ return 1; }

#ifdef AUTONOMOUS_TEST
#include "test.h"
int main(){
	Autonomous a;
	test_executive(a);
}
#endif 
