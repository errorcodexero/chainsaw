#ifndef PERSISTENT_TRACKER_H
#define PERSISTENT_TRACKER_H

#include "maybe.h"
#include <vector>

template<typename T>
class Persistent_tracker{
	unsigned keep;
	std::vector<T> values;

	public:
	Persistent_tracker(); 
	Persistent_tracker(unsigned); 
	void update(T);
	Maybe<T> average()const;
	Maybe<T> max()const;
	Maybe<T> min()const;

	std::vector<T> get()const;
	void discard(unsigned);	
	
	template<typename U>
	friend std::ostream& operator<<(std::ostream&,Persistent_tracker<U> const&);
};

#endif
