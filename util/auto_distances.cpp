#include "auto_distances.h"
#include <fstream>

using namespace std;

//All are in inches for distances / degrees for turns
#define DASH                240
#define ALIGN_COMP          12

#define BASELINE_DIST       93.25
#define BASELINE_EXTRA      12
#define BASELINE_EXT_DIST   144

#define MIDDLE_DIST         114
#define MIDDLE_EXT_DIST_1   120
#define MIDDLE_EXT_DIST_2  -12
#define MIDDLE_EXT_DIST_3   36
#define MIDDLE_EXT_TURN     45

#define BOILER_DIST         115
#define BOILER_RAM_DIST     6
#define BOILER_TURN         40
#define BOILER_EXT_DIST_1   60
#define BOILER_EXT_DIST_2  -24
#define BOILER_EXT_TURN     40

#define LOADING_DIST        115
#define LOADING_RAM_DIST    6
#define LOADING_TURN        33
#define LOADING_EXT_TURN_1  31
#define LOADING_EXT_TURN_2  40
#define LOADING_EXT_DIST   -12

Auto_distances::Auto_distances():
	dash(DASH),
	align_comp(ALIGN_COMP),
	baseline_dist(BASELINE_DIST),
	baseline_extra(BASELINE_EXTRA),
	baseline_ext_dist(BASELINE_EXT_DIST),
	middle_dist(MIDDLE_DIST),
	middle_ext_dist_1(MIDDLE_EXT_DIST_1),
	middle_ext_dist_2(MIDDLE_EXT_DIST_2),
	middle_ext_dist_3(MIDDLE_EXT_DIST_3),
	middle_ext_turn(MIDDLE_EXT_TURN),
	boiler_dist(BOILER_DIST),
	boiler_ram_dist(BOILER_RAM_DIST),
	boiler_turn(BOILER_TURN),
	boiler_ext_dist_1(BOILER_EXT_DIST_1),
	boiler_ext_dist_2(BOILER_EXT_DIST_2),
	boiler_ext_turn(BOILER_EXT_TURN),
	loading_dist(LOADING_DIST),
	loading_ram_dist(LOADING_RAM_DIST),
	loading_turn(LOADING_TURN),
	loading_ext_turn_1(LOADING_EXT_TURN_1),
	loading_ext_turn_2(LOADING_EXT_TURN_2),
	loading_ext_dist(LOADING_EXT_DIST)
{}

ostream& operator<<(ostream& o,Auto_distances const& a){
	o<<"Auto_distances( ";
	#define X(TYPE,NAME) o<<""#NAME<<":"<<a.NAME<<" ";
	AUTO_DISTANCES_ITEMS(X)
	#undef X
	return o<<")";
}

bool operator<(Auto_distances const& a,Auto_distances const& b){
	#define X(TYPE,NAME) if(a.NAME<b.NAME) return 1; if(b.NAME<a.NAME) return 0;
	AUTO_DISTANCES_ITEMS(X)
	#undef X
	return 0;
}

bool operator==(Auto_distances const& a,Auto_distances const& b){
	#define X(TYPE,NAME) if(a.NAME!=b.NAME) return 0;
	AUTO_DISTANCES_ITEMS(X)
	#undef X
	return 1;
}

bool operator!=(Auto_distances const& a,Auto_distances const& b){
	return !(a==b);
}

#ifdef AUTO_DISTANCES_TEST
static const char *AUTO_DISTANCES_FILE="auto_distances.txt";
#else
static const char *AUTO_DISTANCES_FILE="/home/lvuser/auto_distances.txt";
#endif

Auto_distances read_auto_distances(){
	Auto_distances r;
	ifstream f(AUTO_DISTANCES_FILE);
	string s;
	//note: if file is incomplete/corrupted will just get as much data as it can.
	#define X(TYPE,NAME) if(f.good()){ getline(f,s); r.NAME=atof(s.c_str()); }
	AUTO_DISTANCES_ITEMS(X)
	#undef X
	return r;
}

void write_auto_distances(Auto_distances c){
	ofstream f(AUTO_DISTANCES_FILE);
	#define X(TYPE,NAME) if(f.good()) f<<c.NAME<<"\n";
	AUTO_DISTANCES_ITEMS(X)
	#undef X
}

#ifdef AUTO_DISTANCES_TEST
Auto_distances rand(Auto_distances*){
	Auto_distances r;
	#define X(TYPE,NAME) r.NAME=rand()%10;
	AUTO_DISTANCES_ITEMS(X)
	#undef X
	return r;
}

void test_auto_distances_rw(){
	auto a=rand((Auto_distances*)nullptr);
	cout<<"a: "<<a<<endl;
	write_auto_distances(a);
	auto b=read_auto_distances();
	cout<<"b: "<<b<<endl;
	assert(a==b);
	Auto_distances c;
	write_auto_distances(c);
}

int main(){
	test_auto_distances_rw();
}
#endif
