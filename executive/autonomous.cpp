#include "autonomous.h"

#include "teleop.h" 
#include "chain.h"
#include "step.h"

using namespace std; 

double deg_to_rad(double deg){
	return deg/180*PI;
}

const Inch BUMPER_LENGTH = 3.0;//inches thickness of one bumper

const Inch ROBOT_LENGTH = 28.0 + 1 * BUMPER_LENGTH; //inches from front to back //TODO: change to match number of sides with bumpers

Executive make_test_step(auto a){
	return Executive{Chain{
		Step{a},
		Executive{Teleop{}}
	}};
}

Executive get_auto_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};

	////////////////////////////
	//
	// Parts of other autonomous modes
	// 

	//for when just want to run across the field at the end of autonomous
	static const Inch DASH_DIST = 20*12;
	Executive dash{Chain{
		Step{Drive_straight{DASH_DIST}},
		Executive{Teleop{}}
	}};

	static const Inch EXTENDED_GEAR_LENGTH = 7.6;//how far the gear extends out of the robot when deployed outward
	static const Inch ALIGN_DIST = 12;//decreases distance from the drive distance to compinsate for the fact that vision doesnt work well at close distances
	//////////////////////////
	//
	// Full autonomous modes
	//
	
	//Auto mode which does nothing
	Executive auto_null{Teleop{}};

	//Auto mode for crossing the baseline
	static const Inch BASELINE_DIST = 7 * 12 + 9.25;//distance from baseline to alliance wall // from testing
	Executive auto_baseline{Chain{
		Step{Drive_straight{BASELINE_DIST + 12}},//move a little farther to give us some room for error
		Executive{Teleop{}}
	}};

	//Scores a gear on the middle peg and then stops
	static const Inch DIST_TO_MIDDLE_PEG = 114;//distance from alliance wall to the middle peg //TODO: find out correct distance
	Executive auto_score_gear_middle{Chain{
		Step{Combo{
			Step{Drive_straight{DIST_TO_MIDDLE_PEG - SCORE_GEAR_APPROACH_DIST - ROBOT_LENGTH - EXTENDED_GEAR_LENGTH-ALIGN_DIST}},
			Step{Lift_gear()}
		}},
		Executive{Chain{
			Step{Combo{
				Step{Align()},
				Step{Drop_collector()}
			}},
			Executive{Chain{
				Step{Combo{
					Step{Drive_straight{ALIGN_DIST}},
					Step{Lift_gear()},
				}},
				Executive{Chain{
					Step{Score_gear()},
					Executive{Teleop()}
				}}
			}}
		}}
	}};

	//Score a gear on the boiler-side peg
	static const Inch FIRST_DRIVE_DIST_BOILER = (133 - ROBOT_LENGTH) + .5 * ROBOT_LENGTH;//centers the robot on the turning point to align with gear peg //from testing
	Executive auto_score_gear_boiler_side{Chain{
		Step{Combo{
			Step{Drive_straight{FIRST_DRIVE_DIST_BOILER}},
			Step{Lift_gear()}
		}},
		Executive{Chain{
			Step{Combo{
				Step{Turn{deg_to_rad(40)}},//from testing
				Step{Lift_gear()}
			}},
			Executive{Chain{
				Step{Combo{
					Step{Drive_straight{7}},//drive forward a bit so score_gear can take over
					Step{Lift_gear()}
				}},
				Executive{Chain{
					Step{Score_gear()},
					Executive{Teleop()}
				}}
			}}
		}}
	}};

	//Crosses the baseline and continues driving to the other side of the field
	Executive auto_baseline_extended{Chain{
		Step{Drive_straight{12*12}},
		dash
	}};
	
	//scores gear on the boilder-side of the field and then drives towards the other side of the field
	Executive auto_score_gear_boiler_side_extended{Chain{
		Step{Drive_straight{5*12}},
		Executive{Chain{
			Step{Turn{deg_to_rad(40)}},
			Executive{Chain{
				Step{Score_gear()},
				Executive{Chain{
					Step{Drive_straight{-2 * 12}},
					Executive{Chain{
						Step{Turn{deg_to_rad(-40)}},
						dash
					}}
				}}
			}}
		}}
	}};

	//scores a gear on the loading station-side peg
	static const Inch FIRST_DRIVE_DIST_LOADING = (127 - ROBOT_LENGTH) + .5 * ROBOT_LENGTH;
	Executive auto_score_gear_loading_station_side{Chain{
		Step{Drive_straight{FIRST_DRIVE_DIST_LOADING}},
		Executive{Chain{
			Step{Turn{deg_to_rad(-33)}},
			Executive{Chain{
				Step{Drive_straight{6}},
				Executive{Chain{
					Step{Score_gear()},
					Executive{Teleop()}
				}}
			}}
		}}
	}};
	
	//Gear Loading Extended
	Executive auto_score_gear_loading_station_side_extended{Chain{
		Step{Drive_straight{FIRST_DRIVE_DIST_LOADING}},
		Executive{Chain{
			Step{Turn{deg_to_rad(-31)}},
			Executive{Chain{
				Step{Score_gear()},
				Executive{Chain{
					Step{Drive_straight{-12}},
					Executive{Chain{
						Step{Turn{deg_to_rad(40)}},
						dash
					}}
				}}
			}}
		}}
	}};
	
	//Gear mid Extended right (goes around the airship to the right)
	Executive auto_score_gear_middle_extended_right{Chain{
		Step{Drive_straight{10*12}},
		Executive{Chain{
			Step{Score_gear()},
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

	//Gear mid Extended left (goes around airship to the left)
	Executive auto_score_gear_middle_extended_left{Chain{
		Step{Drive_straight{10*12}},
		Executive{Chain{
			Step{Score_gear()},
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

	if(info.panel.in_use){
		switch(info.panel.auto_select){ 
			case 0: 
			//	return auto_null;//TODO: make sure this is un-commented for competition

				////////////////////////////
				//
				// Tests for different steps
				//
				//return make_test_step(Drive_straight{9*12});
				//return make_test_step(Step{Score_gear()});
				//return make_test_step(Turn{PI*2});
				return make_test_step(Align());
			case 1: 
				return auto_baseline;
			case 2: 
				return auto_baseline_extended;		
			case 3: 
				return auto_score_gear_boiler_side;
			case 4: 
				return auto_score_gear_boiler_side_extended;
			case 5: 
				return auto_score_gear_loading_station_side;
			case 6: 
				return auto_score_gear_loading_station_side_extended;
			case 7: 
				return auto_score_gear_middle;
			case 8: 
				return auto_score_gear_middle_extended_right;
			case 9: 
				return auto_score_gear_middle_extended_left;
			case 10:
				//Vision test (TEMP)
				return make_test_step(Align());
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
