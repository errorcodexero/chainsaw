#include "autonomous.h"

#include "teleop.h" 
#include "chain.h"
#include "step.h"

using namespace std; 

double deg_to_rad(double deg){
	return deg/180*PI;
}

//using Geardrop=Drive_straight{0}; //TODO: Fixme
auto Geardrop(){ return Drive_straight{0}; }

Executive auto_mode_convert(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};

	//for when just want to run across the field at the end of autonomous
	Executive dash{Chain{
		Step{Drive_straight{12*20}},
		Executive{Teleop{}}
	}};

	Executive backoff{Chain{
		Step{Drive_straight{-1*12}},
		Executive{Teleop{}}
	}};

	Executive auto_null{Teleop{}};

	Executive auto_baseline{Chain{
		Step{Drive_straight{12*12}},
		Executive{Teleop{}}
	}};
	
	Executive drive_straight_test{Chain{//used to test the Step Drive_straight
		Step{Drive_straight{3*12}},
		Executive{Teleop{}}
	}};

	Executive turn_test{Chain{//used to test the Step Turn
		Step{Turn{PI/2}},
		Executive{Teleop{}}
	}};

	if (info.panel.in_use) {
		switch(info.panel.auto_select){ 
			case 0: //Do Nothing
				return auto_null;
				//return drive_straight_test; 
				//return turn_test;
			case 1: //Baseline
				return auto_baseline;
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
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			default:
				return auto_null;
		}
	}
	return Executive{Teleop()};//Default Executive if no panel exists (noramlly Teleop)
}

Executive Autonomous::next_mode(Next_mode_info info){
	static const Time DELAY = 0.0;//seconds, TODO: base it off of the dial on the OI?
	if(!info.autonomous) return Executive{Teleop()};
	if(info.since_switch > DELAY) return auto_mode_convert(info);
	return Executive{Autonomous()};
}

Toplevel::Goal Autonomous::run(Run_info){	
	return {};//nothing, just waits
}

bool Autonomous::operator==(Autonomous const&)const{ return 1; }

#ifdef AUTONOMOUS_TEST
#include "test.h"
int main(){
	Autonomous a;
	test_executive(a);
}
#endif 
