#include "shooter_feed.h"
#include <stdlib.h>

using namespace std;

#define SHOOTER_FEED_RELAY 1 //This is going to be deleted anyway 

ostream& operator<<(ostream& o, Shooter_feed::Goal a){
	#define X(name) if(a==Shooter_feed::Goal::name)return o<<""#name;
	X(IN) X(OUT) X(OFF)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Shooter_feed a){ return o<<"Shooter_feed("<<a.estimator.get()<<")";}

bool operator==(Shooter_feed::Output_applicator, Shooter_feed::Output_applicator){return 1;}

bool operator==(Shooter_feed a, Shooter_feed b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Shooter_feed a, Shooter_feed b){ return !(a==b);}

Robot_outputs Shooter_feed::Output_applicator::operator()(Robot_outputs r, Shooter_feed::Output out)const{
	r.relay[SHOOTER_FEED_RELAY] = [&]{
		switch(out){
			case Shooter_feed::Output::OUT:
				return Relay_output::_10;
			case Shooter_feed::Output::OFF:
				return Relay_output::_00;
			case Shooter_feed::Output::IN:
				return Relay_output::_01;	
			default:
				assert(0);
		}
	}();
	return r;
}

Shooter_feed::Output Shooter_feed::Output_applicator::operator()(Robot_outputs r)const{
	switch(r.relay[SHOOTER_FEED_RELAY]){
		case Relay_output::_10:
			return Shooter_feed::Output::OUT;
		case Relay_output::_00:
			return Shooter_feed::Output::OFF;
		case Relay_output::_01:
			return Shooter_feed::Output::IN;
		default:
			assert(0);
	}
}
	
set<Shooter_feed::Goal> examples(Shooter_feed::Goal*){ 
	return {Shooter_feed::Goal::OUT,Shooter_feed::Goal::OFF,Shooter_feed::Goal::IN};
}

Shooter_feed::Output control(Shooter_feed::Status_detail, Shooter_feed::Goal goal){
	return goal;
}

bool ready(Shooter_feed::Status, Shooter_feed::Goal){ return true;}

#ifdef SHOOTER_FEED_TEST
#include "formal.h"

int main(){
	Shooter_feed a;
	tester(a);
}

#endif
