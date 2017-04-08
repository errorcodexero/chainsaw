#include "persistent_tracker.h"
#include "util.h"

using namespace std;

template<typename T>
Persistent_tracker<T>::Persistent_tracker(unsigned a):keep(a){}

template<typename T>
Persistent_tracker<T>::Persistent_tracker():Persistent_tracker(5){}

template<typename T>
vector<T> Persistent_tracker<T>::get()const{
	return values;
}

template<typename T>
void Persistent_tracker<T>::discard(unsigned i){
	assert(i < values.size());
	values.erase(values.begin() + i);
}

template<typename T>
Maybe<T> Persistent_tracker<T>::average()const{
	if(values.empty()) return Maybe<T>();
	Maybe<T> average;
	T sum = {}; 
	for(T a: values){
		sum = sum + a;
	}
	average = sum / values.size();
	return average;
}


template<typename T>
Maybe<T> Persistent_tracker<T>::max()const{
	if(values.empty()) return Maybe<T>();
	Maybe<T> max;
	for(T a: values){
		if(!max || a > *max) max = a;
	}
	return max;
}

template<typename T>
Maybe<T> Persistent_tracker<T>::min()const{
	if(values.empty()) return Maybe<T>();
	Maybe<T> min;
	for(T a: values){
		if(!min || a < *min) min = a;
	}
	return min;
}


template<typename T>
void Persistent_tracker<T>::update(T v){
	if(values.size() > keep){
		values.erase(values.begin());
	}
	values.push_back(v);
}

template<typename T>
ostream& operator<<(ostream& o,Persistent_tracker<T> const& a){
	o<<"Persistent_tracker(";
	o<<"keep:"<<a.keep;
	o<<" values:"<<a.values;
	o<<")";
	return o;//TODO
}

#ifdef PERSISTENT_TRACKER_TEST

int main(){
	Persistent_tracker<int> a;
	for(int i: range(100)){
		cout<<a<<" avg:"<<a.average()<<" max:"<<a.max()<<" min:"<<a.min()<<" get:"<<a.get()<<"\n";
		a.update(i);
		for(unsigned i = 0; i < a.get().size(); i++){
			if(a.get()[i] % 2 == 0) a.discard(i);
		}
	}
}

#endif
