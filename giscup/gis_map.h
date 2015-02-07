#pragma once

#include <vector>
#include "gis_segment.h"
#include "gis_container.h"

using namespace std;

class gis_map {
public:
	gis_map();
	~gis_map();
	unsigned int add_segment(unsigned int edgeid, double latitude1, double longitude1, double latitude2, double longitude2);
private:
	gis_container container;
	vector<gis_segment> segment;
};

