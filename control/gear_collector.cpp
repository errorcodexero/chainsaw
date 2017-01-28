#include "gear_collector.h"

using namespace std;

Gear_collector::Goal::Goal():gear_grabber(Gear_grabber::Goal::OPEN),gear_lifter(Gear_lifter::Goal::DOWN){}

Gear_collector::Goal::Goal(Gear_grabber::Goal g,Gear_lifter::Goal l):gear_grabber(g),gear_lifter(l){}

ostream& operator<<(ostream& o,Gear_collector const&){
	return o<<"Gear_collector()";
}

ostream& operator<<(ostream& o,Gear_collector::Output const& a){
	o<<"Gear_collector::Output( ";
	#define X(A,B) o<<a.B<<" ";
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Gear_collector::Input const& a){
	o<<"Gear_collector::Input( ";
	#define X(A,B) o<<a.B<<" ";
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Gear_collector::Status const& a){
	o<<"Gear_collector::Status( ";
	#define X(A,B) o<<a.B<<" ";
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Gear_collector::Status_detail const& a){
	o<<"Gear_collector::Status_detail( ";
	#define X(A,B) o<<a.B<<" ";
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Gear_collector::Goal const& a){
	o<<"Gear_collector::Goal( ";
	#define X(A,B) o<<a.B<<" ";
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return o<<")";
}

Robot_inputs Gear_collector::Input_reader::operator()(Robot_inputs a,Gear_collector::Input b)const{
	#define X(A,B) a=B(a,b.B);
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return a;
}

Gear_collector::Input Gear_collector::Input_reader::operator()(Robot_inputs const& a)const{
	return {
		#define X(A,B) B(a),
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	};
}

bool operator==(Gear_collector const&,Gear_collector const&){
	return 1;
}

bool operator!=(Gear_collector const& a,Gear_collector const& b){
	return !(a==b);
}

bool operator<(Gear_collector::Output a,Gear_collector::Output b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 0;
}

bool operator<(Gear_collector::Input,Gear_collector::Input)nyi

bool operator<(Gear_collector::Goal a,Gear_collector::Goal b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 0;
}

bool operator==(Gear_collector::Status a,Gear_collector::Status b){
	#define X(A,B) if(a.B!=b.B) return 0;
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 1;
}

bool operator!=(Gear_collector::Status a,Gear_collector::Status b){
	return !(a==b);
}

bool operator==(Gear_collector::Estimator a,Gear_collector::Estimator b){
	#define X(A,B) if(a.B!=b.B) return 0;
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 1;
}

bool operator!=(Gear_collector::Estimator a,Gear_collector::Estimator b){
	return !(a==b);
}

bool operator==(Gear_collector::Output a,Gear_collector::Output b){
	#define X(A,B) if(a.B!=b.B) return 0;	
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 1;
}

bool operator!=(Gear_collector::Input a,Gear_collector::Input b){
	return !(a==b);
}

bool operator==(Gear_collector::Input a,Gear_collector::Input b){
	#define X(A,B) if(a.B!=b.B) return 0;	
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 1;
}

bool operator!=(Gear_collector::Output a,Gear_collector::Output b){
	return !(a==b);
}

bool operator<(Gear_collector::Status a,Gear_collector::Status b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 0;
}

bool operator==(Gear_collector::Status_detail a,Gear_collector::Status_detail b){
	#define X(A,B) if(a.B!=b.B) return 0;	
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 1;
}

bool operator!=(Gear_collector::Status_detail a,Gear_collector::Status_detail b){
	return !(a==b);
}

bool operator<(Gear_collector::Status_detail a,Gear_collector::Status_detail b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 0;
}

Gear_collector::Status status(Gear_collector::Status_detail const& a){
	return {
		#define X(A,B) status(a.B),
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	};
}

Gear_collector::Output control(Gear_collector::Status_detail const& st,Gear_collector::Goal const& goal){
	return {
		#define X(A,B) control(st.B,goal.B),
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	};
}

void Gear_collector::Estimator::update(Time a,Input b,Output c){
	#define X(A,B) B.update(a,b.B,c.B);
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
}

Gear_collector::Status_detail Gear_collector::Estimator::get()const{
	return Gear_collector::Status_detail{
		#define X(A,B) B.get(),
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	};
}

Gear_collector::Output Gear_collector::Output_applicator::operator()(Robot_outputs const& a)const{
	return Gear_collector::Output{
		#define X(A,B) B(a),
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	};
}

Robot_outputs Gear_collector::Output_applicator::operator()(Robot_outputs a,Output const& b)const{
	#define X(A,B) a=B(a,b.B);
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return a;
}

set<Gear_collector::Input> examples(Gear_collector::Input*){
	return {{
		#define X(A,B) example((A::Input*)nullptr),
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	}};
}

set<Gear_collector::Status_detail> examples(Gear_collector::Status_detail*){
	/*return {{
		#define X(A,B) example((A::Status_detail*)nullptr),
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	}};*/
	set<Gear_collector::Status_detail> r;
	for(auto a:examples((Gear_grabber::Status_detail*)0)){
		for(auto b:examples((Gear_lifter::Status_detail*)0)){
			r|=Gear_collector::Status_detail{a,b};
		}
	}
	return r;
}

/*template<typename A,typename B,typename C>
auto cross(A a,B b,C c){

}*/

set<Gear_collector::Goal> examples(Gear_collector::Goal*){
	/*return {{
		#define X(A,B) example((A::Goal*)nullptr),
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	}};*/
	set<Gear_collector::Goal> r;
	for(auto a:examples((Gear_grabber::Goal*)0)){
		for(auto b:examples((Gear_lifter::Goal*)0)){
			r|=Gear_collector::Goal{a,b};
		}
	}
	return r;
}

set<Gear_collector::Output> examples(Gear_collector::Output*){
	set<Gear_collector::Output> r;
	for(auto a:examples((Gear_grabber::Output*)0)){
		for(auto b:examples((Gear_lifter::Output*)0)){
			r|=Gear_collector::Output{a,b};
		}
	}
	return r;
	//return {{}};
	/*	#define X(A,B) example((A::Output*)nullptr),
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	}};*/
}

set<Gear_collector::Status> examples(Gear_collector::Status*){
	set<Gear_collector::Status> r;
	for(auto a:examples((Gear_grabber::Status*)nullptr)){
		for(auto b:examples((Gear_lifter::Status*)nullptr)){
			r|=Gear_collector::Status{a,b};
		}
	}
	return r;
}

bool ready(Gear_collector::Status const& st,Gear_collector::Goal const& goal){
	#define X(A,B) if(!ready(st.B,goal.B)) return 0;
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
	return 1;
}

#ifdef GEAR_COLLECTOR_TEST
#include "formal.h"
int main(){
	Tester_mode mode;
	mode.check_outputs_exhaustive=0;
	Gear_collector a;
	tester(a,mode);
	return 0;
}
#endif
