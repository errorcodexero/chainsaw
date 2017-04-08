#include "persistence_tracker.h"
#include <cmath>
#include <iomanip>

using namespace std;

Persistence_tracker::Persistence_tracker(){}

ostream& operator<<(ostream& o,Persistence_tracker const& a){
	o<<"Persistence_tracker(";
	return o<<")";
}

#ifdef PERSISTENCE_TRACKER_TEST

int main(){
	Persistence_tracker<int> a;
}

#endif
