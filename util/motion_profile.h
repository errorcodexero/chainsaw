#ifndef MOTION_PROFILE_H
#define MOTION_PROFILE_H
#include <functional>
#include <assert.h>

class Motion_profile{
	double goal;//inches
	double vel_modifier;
	
	public:
	double target_speed(const double);
	void set_goal(double);

	Motion_profile();
	Motion_profile(double,double);
	friend std::ostream& operator<<(std::ostream&,Motion_profile const&);
};

std::ostream& operator<<(std::ostream&,Motion_profile const&);

#endif
