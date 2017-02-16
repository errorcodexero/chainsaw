#include "delay.h"

#include "teleop.h" 
#include "auto_null.h"
#include "auto_distance.h"
#include "auto_forward.h"
#include "auto_baseline.h"
#include "auto_gearboiler.h"
#include "auto_gearloading.h"
#include "auto_gearmid.h"
#include "auto_baselineext.h"

using namespace std; 
using Mode=Executive;

Executive auto_mode_convert(Next_mode_info info){
	cout << "panel in use " << info.panel.in_use << "\n";
	if (info.panel.in_use) {
		switch(info.panel.auto_select){ 
			case 1: 
				return Executive{Auto_null()};
			case 2:
				return Executive{Auto_baseline({0,0,0})};
			case 3:
				return Executive{Auto_baseline({0,0,0})};
			case 4:
				return Executive{Auto_gearboiler_topeg()};
			case 5:
				return Executive{Auto_gearboiler_topeg()};
			case 6:
				//return Executive{Auto_gearloading_topeg({0,0})};
			case 7:
				//return Executive{Auto_gearloading_topeg({0,0})};
			case 8:
				//return Executive{Auto_gearmid_topeg({0,0})};
			case 9:
				//return Executive{Auto_gearmid_topeg({0,0})};
			case 0:
			default:
				return Executive{Auto_null()};
		}
	}
	return Executive{Auto_baseline{{0,0,0}}};
}

Mode Delay::next_mode(Next_mode_info info){
	cout << "DELAY FLAG" << "\n";
	if(!info.autonomous) return Mode{Teleop()};
	cout << "info " << info.since_switch << "\n";
	if(info.since_switch > .5) return auto_mode_convert(info);
	return Mode{Delay()};
}

Toplevel::Goal Delay::run(Run_info){
	
	return {};
}

bool Delay::operator==(Delay const&)const{ return 1; }

#ifdef DELAY_TEST
#include "test.h"
int main(){
	Delay a;
	test_executive(a);
}
#endif 
