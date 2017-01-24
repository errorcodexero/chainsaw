#include "gear_lift.h"

using namespace std;

Gear_lift::Gear_lift():
	output_applicator(0)//this IO is totally made up.
{}

ostream& operator<<(ostream& o,Gear_lift const&){
	return o<<"Gear_lift(...)";
}

#ifdef GEAR_LIFT_TEST
#include "formal.h"

int main(){
	Gear_lift g;
	tester(g);
}
#endif
