#ifndef BOUND_TRACKER_H
#define BOUND_TRACKER_H

#include <iosfwd>
#include <iostream>

//this would be easy to make a template on the value type.

template<typename T>
struct Bound_tracker{//TODO: make variables private
	T min,max;
	bool data;
	
	Bound_tracker():data(false){
		//min = (strtod("NAN",NULL));//TODO
		//max = (strtod("NAN",NULL));//TODO
	}
	
	void update(T const& v){
		if(data){
			//std::min and std::max don't work right on the cRIO. //TODO see if this works on the roboRIO
			//min=std::min(min,v);
			//max=std::max(max,v);
			if(v<min) min=v;
			if(v>max) max=v;
		}else{
			min = v;
			max = v;
			data= true;
		}
	}

	//friend std::ostream& operator<<(std::ostream&,Bound_tracker<T>){
	
};

template<typename T>
std::ostream& operator<<(std::ostream& o,Bound_tracker<T> a){
	o<<"Bound_tracker(";
	if(a.data){
		o<<"min:"<<a.min<<" max:"<<a.max;
	}else{
		o<<"Not Set";
	}
	return o<<")";
}

#endif
