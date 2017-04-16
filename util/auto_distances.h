#ifndef AUTO_DISTANCES_H
#define AUTO_DISTANCES_H

#include "quick.h"
#include "interface.h"

//Distances are in inches
#define AUTO_DISTANCES_ITEMS(X) \
	X(double,dash) \
	X(double,align_comp) \
	X(double,baseline_dist) \
	X(double,baseline_extra) \
	X(double,baseline_ext_dist) \
	X(double,middle_dist) \
	X(double,middle_ext_dist_1) \
	X(double,middle_ext_dist_2) \
	X(double,middle_ext_dist_3) \
	X(double,middle_ext_turn) \
	X(double,boiler_dist) \
	X(double,boiler_ram_dist) \
	X(double,boiler_turn) \
	X(double,boiler_ext_dist_1) \
	X(double,boiler_ext_dist_2) \
	X(double,boiler_ext_turn) \
	X(double,loading_dist) \
	X(double,loading_ram_dist) \
	X(double,loading_turn) \
	X(double,loading_ext_turn_1) \
	X(double,loading_ext_turn_2) \
	X(double,loading_ext_dist)

struct Auto_distances{
	STRUCT_MEMBERS(AUTO_DISTANCES_ITEMS)
	Auto_distances();
};

bool operator<(Auto_distances const&,Auto_distances const&);
bool operator==(Auto_distances const&,Auto_distances const&);
bool operator!=(Auto_distances const&,Auto_distances const&);
std::ostream& operator<<(std::ostream&,Auto_distances const&);

Auto_distances read_auto_distances();
void write_auto_distances(Auto_distances);

#endif
