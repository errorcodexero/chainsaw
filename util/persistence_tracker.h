#ifndef PERSISTENCE_TRACKER_H
#define PERSISTENCE_TRACKER_H

#include "interface.h"

template<typename T>
class Persistence_tracker{
	unsigned keep;
	std::vector<T> values;

	public:
	Persistence_tracker(); 
	void update(double);
	Maybe<T> average()const;
	Maybe<T> max()const;
	Maybe<T> min()const;

	friend std::ostream& operator<<(std::ostream&,Persistence_tracker const&);
};

#endif
