#ifndef MOTION_PROFILE_H
#define MOTION_PROFILE_H
#include <functional>
#include <assert.h>
template<class T, class Compare>
const T& clamp( const T& v, const T& lo, const T& hi, Compare comp )
{
	return assert( !comp(hi, lo) ),
	comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<class T>
const T& clamp( const T& v, const T& lo, const T& hi )
{
	return clamp( v, lo, hi, std::less<T>() );
}

struct Motion_profile{
	double goal;//inches
	double vel_modifier;
	
	double target_speed(const double);
	void set_goal(double);

	Motion_profile();
	Motion_profile(double,double);	
	
};

#endif
