#ifdef SIM_TEST
#include "toplevel.h"
#include "../util/type.h"
#include "nop.h"
#include <math.h>
#include "main.h"
#include "../util/util.h"

using namespace std;

template<typename T,size_t LEN>
array<pair<size_t,T>,LEN> enumerate(array<T,LEN> a){
	array<pair<size_t,T>,LEN> r;
	for(auto i:range(LEN)){
		r[i]=make_pair(i,a[i]);
	}
	return r;
}

template<typename Input>
struct Nop_sim{
	template<typename Output>
	void update(Time,bool,Output){}

	Input get()const{
		auto e=examples((Input*)0);
		assert(e.size());
		return *begin(e);
	}
};

template<typename T>
ostream& operator<<(ostream& o,Nop_sim<T> const& a){
	return o<<type(a);
}

using Pump_sim=Nop_sim<Pump::Input>;
using Climber_sim=Nop_sim<Climber::Input>;
using Intake_sim=Nop_sim<Intake::Input>;
using Gear_shifter_sim=Nop_sim<Gear_shifter::Input>;
using Arm_sim=Nop_sim<Arm::Input>;
using Gear_grabber_sim=Nop_sim<Gear_grabber::Input>;
using Gear_lifter_sim=Nop_sim<Gear_lifter::Input>;
using Shooter_feed_sim = Nop_sim<Shooter_feed::Input>;

struct Drivebase_sim{
	using Input=Drivebase::Input;
	using Output=Drivebase::Output;
	
	float x=0,y=0,theta=0;//x,y are in distance in feet
	Time last_time =0;
	int ticks_left=2;
	int ticks_right=2;
	float dist_left=0;
	float dist_right=0;
	void update(Time t,bool enable,Output out){
		Time dt=t-last_time;
		last_time=t;
		if(!enable) return;
		float dtheta = (((out.l-out.r)*5/12.5))*6.25;
		float speedl= (out.l)*.2;
		float speedr= (out.r)*.2;
		dist_left+=speedl*dt;
		dist_right+=speedr*dt;
		float dist_traveled=(dist_left+dist_right)/2;
		float dy=dist_traveled*cosf(theta);
		float dx=dist_traveled*sinf(theta);
		ticks_left+=inches_to_ticks(dist_left*12);
		ticks_right+=inches_to_ticks(dist_right*12);
		cout << "dx:"<<dx<< " dy:" << dy << " dt:" << dtheta << "\n";
		y+=dy;
		x+=dx;
		theta+=dtheta;
		cout << "x:" << x << " y:" << y << "\n";
	
	}
	Input get()const{
		auto d=Digital_in::_0;
		auto p=make_pair(d,d);
		return {Drivebase::Input{
			{0,0,0,0,0,0},p,p,{ticks_left,ticks_right}
		}};
	}

};

ostream& operator<<(ostream& o,Drivebase_sim const& a){
	return o << "Drivebase_sim(" << a.x << a.y << a.x << a.theta << ")\n";
}

struct Collector_sim{
	using Input=Collector::Input;
	using Output=Collector::Output;

	#define X(A,B) A##_sim B;
	COLLECTOR_ITEMS(X)
	 #undef X

	 void update(Time t,bool enable,Output out){
		#define X(A,B) B.update(t,enable,out.B);
		COLLECTOR_ITEMS(X)
		#undef X
	}

	Input get()const{
		return Input{
			#define X(A,B) B.get(),
			COLLECTOR_ITEMS(X)
			#undef X
		};
	}
};

struct Gear_collector_sim{
	using Input=Gear_collector::Input;
	using Output=Gear_collector::Output;

	#define X(A,B) A##_sim B;
	GEAR_COLLECTOR_ITEMS(X)
	#undef X

	void update(Time t,bool enable,Output out){
		#define X(A,B) B.update(t,enable,out.B);
		GEAR_COLLECTOR_ITEMS(X)
		#undef X
	}

	Input get()const{
		return Input{
			#define X(A,B) B.get(),
			GEAR_COLLECTOR_ITEMS(X)
			#undef X
		};
	}
};

struct Toplevel_sim{
	using Input=Toplevel::Input;
	using Output=Toplevel::Output;

	#define X(A,B,C) A##_sim B;
	TOPLEVEL_ITEMS
	#undef X

	void update(Time t,bool enable,Output out){
		#define X(A,B,C) B.update(t,enable,out.B);
		TOPLEVEL_ITEMS
		#undef X
	}

	Input get()const{
		return {
			#define X(A,B,C) B.get(),
			TOPLEVEL_ITEMS
			#undef X
		};
	}
};

template<typename SIMULATOR,typename DEVICE>
void simulate(SIMULATOR sim,DEVICE device){
	static const Time TIMESTEP=.1;//this will probably change to .01 or .02 later.
	auto in=sim.get();
	for(Time t=0;t<2;t+=TIMESTEP){
		//TODO: Make the simulator go 2014 style where you only get "significant" changes printed
		//device.estimator.update(
		
	}
}

template<typename F>
void visit(F f,Collector_sim a){
	#define X(A,B) f(""#B,a.B);
	COLLECTOR_ITEMS(X)
	#undef X
}

template<typename F>
void visit(F f,Gear_collector_sim a){
	#define X(A,B) f(""#B,a.B);
	GEAR_COLLECTOR_ITEMS(X)
	#undef X
}

template<typename F>
void visit(F f,Toplevel_sim a){
	#define X(A,B,C) f(""#B,a.B);
	TOPLEVEL_ITEMS
	#undef X
}

template<typename F,typename T>
void visit(F f,T t){
	f.terminal(t);
}

template<typename F,typename T>
void visit(F,Nop_sim<T>){}

/*void visit(auto f,Encoder_info const& a){

}*/

template<typename T,size_t LEN>
void visit(auto f,array<T,LEN> const& a){
	//mapf([&](auto p){ f(as_string(p.first),p.second); },enumerate(a));
	for(auto p:enumerate(a)){
		f(as_string(p.first),p.second);
	}
}

template<typename A,typename B>
void visit(auto f,pair<A,B> p){
	f("first",p.first);
	f("second",p.second);
}

#define VISIT(A,B) f(""#B,a.B);

void visit(auto f,Drivebase::Input const& a){
	DRIVEBASE_INPUT(VISIT)
}

template<typename F>
void visit(F f,Toplevel::Input const& a){
	#define X(A,B,C) f(""#B,a.B);
	TOPLEVEL_ITEMS
	#undef X
}

void visit(auto f,Nop::Input){}

struct Display{
	int indent_level=0;

	string get_label(auto a)const{
		return type(a);
	}

	/*auto get_label(int a)const{ return to_string(a); }
	auto get_label(bool a)const{ return to_string(a); }
	auto get_label(double a)const{ return to_string(a); }*/
	
	void indent()const{
		for(auto _:range(indent_level)){
			(void)_;
			cout<<"\t";
		}
	}

	void operator()(string label,auto a)const{
		indent();
		cout<<label<<"\n";
		visit(Display{indent_level+1},a);
	}

	void terminal(auto a)const{
		indent();
		cout<<a<<"\n";	
	}
};

template<typename T>
void sim_display(T t){
	/*auto label=get_top(t);
	cout<<label<<"\n";*/
	Display{}("robot",t);
}

int main(){
	{
		Toplevel_sim sim;
		sim_display(sim);
		sim_display(sim.get());
		sim_display(example((Toplevel::Output*)0));
	}
	cout<<"\n================================================\n";
	{
		Toplevel_sim sim;
		Main m;
		Robot_inputs all;	
		all.robot_mode.autonomous=true;
		all.robot_mode.enabled=true;
		all.joystick[2].axis[1]=1;
		auto robotinput = m.toplevel.input_reader(all,sim.get());
		cout << "robot mode " << all.robot_mode << "\n";	
		static const Time TIMESTEP=.1;
		robotinput.robot_mode.autonomous=true;
		robotinput.robot_mode.enabled=true;
		for(Time t=0;t<20;t+=TIMESTEP){
			cout << "Main " << m << "\n";
			cout<<t<<"\t"<<sim.get()<<"\n";
			auto out=m(robotinput);
			
			//auto out=example((Toplevel::Output*)0);
			/*Toplevel::Goal goal;
			goal.drive.left=1;
			goal.drive.right=1;
			auto status_detail=m.toplevel.estimator.get();
			auto out=control(status_detail,goal);*/
			cout <<"out "  << out << "\n";
			sim.update(t,1,m.toplevel.output_applicator(out));
			m.toplevel.estimator.update(t,sim.get(),m.toplevel.output_applicator(out));
		}
	}
	return 0;	
}

#endif
