#include "motion_profile.h"
#include <iostream>
#include <assert.h>
#include <algorithm>
#include "interface.h"
#include "util.h"

using namespace std;

double Motion_profile::target_speed(const double current){
	float error = goal-current;
	float motorv = .2; //error*vel_modifier;//TODO: use vel_modifier
	const float max=.2;//TODO: change to 1.0
	//clamp(motorv,-max,max); //TODO fix so that it will return proper values.
	return clamp((error/12)*motorv,-max,max);
}

void Motion_profile::set_goal(double g){
	goal=g;
}

Motion_profile::Motion_profile():goal(0),vel_modifier(.02){}
Motion_profile::Motion_profile(double g, double m):goal(g),vel_modifier(m){}

ostream& operator<<(ostream& o,Motion_profile const& a){
	o<<"Motion_profile(";
	o<<"goal:"<<a.goal;
	o<<" velocity modifier:"<<a.vel_modifier;
	return o<<")";
}

#ifdef MOTION_PROFILE_TEST

int main(){
	{
		const double TARGET = 100*12;//inches
		Motion_profile mp(TARGET, .01);	
		
		cout<<"\nSpeed List:\n";
		for(int i=0;i<=TARGET;i+=10){//i is distance in inches
			 cout<<"\tRobot has moved "<<i<< " inches. Goal is "<<TARGET<<" inches. Target speed is "<<mp.target_speed(i)<<" using motion profile:"<<mp<<endl;
		}
	
	}
	{		
		double dis=0; //inches, the current distance of the simulated robot
		
		const double TARGET = 20*12; //inches
		const double MAX_SPEED = 10*12; //inches per second, the maxium speed of the robot that will be slowed down by motion profiling
		Motion_profile mp(TARGET, .01);
	
		const float INCREMENT=.5;//seconds
		
		cout<<"\nSimulation\n";	
		for(Time i=0;i<60;i+=INCREMENT){//i is in seconds
			cout<<"\tTime(seconds):"<<i<<"    Current Distance(inches):"<<dis<<"    Target Distance(inches):"<<TARGET<<"    Target Speed(percent power):"<<mp.target_speed(dis)<<endl;
			if(dis >= (TARGET-1)){
				cout<<"\tRobot has reached the target range"<<endl;
				break;
			}
			dis += INCREMENT*MAX_SPEED*mp.target_speed(dis);
		}
	}	
	return 0;
}

#endif
