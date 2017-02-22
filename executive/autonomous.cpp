#include "autonomous.h"

#include "teleop.h" 
#include "chain.h"
#include "step.h"

using namespace std; 

double deg_to_rad(double deg){
	return deg/180*PI;
}

const Inch SCORE_GEAR_APPROACH_DIST = 18.0;//inches

const Inch ROBOT_LENGTH = 29.0; //inches from front to back

Executive insert_score_gear(Executive last){
	Executive score_gear{Chain{
		Step{Lift_gear()},//lift gear from floor
		Executive{Chain{
			Step{Combo{
				Step{Lift_gear()},
				Step{Drive_straight{SCORE_GEAR_APPROACH_DIST}}
			}},//slide gear onto peg
			Executive{Chain{
				Step{Drop_gear()},//let go of gear
				Executive{Chain{
					Step{Combo{
						Step{Wait{.5}},
						Step{Drop_gear()}
					}},//make sure we're not attached to the gear
					Executive{Chain{
						Step{Combo{
							Step{Drop_gear()},
							Step{Drive_straight{-SCORE_GEAR_APPROACH_DIST}}
						}},//drive back from the peg
						Executive{Chain{
							Step{Drop_collector()},//lower the collector to the floor
							last //what to do after scoring the gear
						}}
					}}
				}}
			}}		
		}}
	}};
	return score_gear;
}


Executive make_test(auto a){
	return Executive{Chain{
		Step{a},
		Executive{Teleop{}}
	}};
}

Executive get_auto_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};

	////////////////////////////
	//
	// Tests for different steps
	//
		
	Executive drive_straight_test = make_test(Drive_straight{7*12});//used to test the Step Drive_straight
	Executive turn_test = make_test(Turn{PI/2});//used to test the Step Turn

	////////////////////////////
	//
	// Parts of other autonomous modes
	// 

	//for scoring a gear after the robot is lined up in front of any of the pegs
	Executive score_gear = insert_score_gear(Executive{Teleop()});

	//for when just want to run across the field at the end of autonomous
	Executive dash{Chain{
		Step{Drive_straight{12*20}},
		Executive{Teleop{}}
	}};

	//////////////////////////
	//
	// Full autonomous modes
	//
	
	//Auto mode which does nothing
	Executive auto_null{Teleop{}};

	//Auto mode for crossing the mode
	Executive auto_baseline{Chain{
		Step{Drive_straight{12*12}},
		Executive{Teleop{}}
	}};

	//Scores a gear on the middle peg and then stops
	Executive gear_drop_mid{Chain{
		Step{Combo{
			Step{Drive_straight{114.3 - SCORE_GEAR_APPROACH_DIST - ROBOT_LENGTH}},//TODO: find out correct distance
			Step{Lift_gear()}
		}},
		score_gear
	}};

	if (info.panel.in_use) {
		switch(info.panel.auto_select){ 
			case 0: //Do Nothing
				//return auto_null;
				//return score_gear;				
				//tests for different steps
				//return make_test(Lift_gear());
				return drive_straight_test; 
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
							score_gear
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
							insert_score_gear(
								Executive{Chain{
									Step{Drive_straight{-12}},
									Executive{Chain{
										Step{Turn{deg_to_rad(-40)}},
										dash
									}}
								}}
							)
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
							insert_score_gear(Executive{Teleop()})
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
							insert_score_gear(
								Executive{Chain{
									Step{Drive_straight{-12}},
									Executive{Chain{
										Step{Turn{deg_to_rad(40)}},
										dash
									}}
								}}
							)
						}}
					}}
				}};	
			case 7: //Gear Mid
				return gear_drop_mid;
			case 8: //Gear mid Extended right
				return Executive{Chain{
					Step{Drive_straight{10*12}},
					insert_score_gear(
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
					)
				}};
			case 9: //Gear mid Extended left
				return Executive{Chain{
					Step{Drive_straight{10*12}},
					insert_score_gear(
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
					)
				}};
			case 10: //Vision test (TEMP)
				return Executive{Chain{
					Step{Drive_straight{5*12}},
					Executive{Chain{
						Step{Turn{deg_to_rad(40)}},
						Executive{Chain{
							Step{Drive_straight{12}}, //approach
							score_gear
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
	static const Time DELAY = 0.0;//seconds, TODO: base it off of the dial on the OI? Or maybe during autonomous wait until we reach a certain air pressure?
	if(!info.autonomous) return Executive{Teleop()};
	if(info.since_switch > DELAY) return get_auto_mode(info);
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
