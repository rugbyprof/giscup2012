#pragma once

#include <vector>
#include "gis_segment.h"

using namespace std;

#define GIS_CONTAINER_SEGMENT_THRESHOLD 30

class gis_container
{
public:
	gis_container(void);
	void setdata(unsigned long latitudemask, unsigned long longitudemask, unsigned int bitindex, double latitudemax, double longitudemax, double latitudemin, double longitudemin);
	~gis_container(void);
	void add_segment(vector<gis_segment> * segment, unsigned int segmentid);
	void get_quadrants(bool quadrant[4], double lat1, double lon1, double lat2, double lon2);
private:
	vector<unsigned int> * mysegmentids;
	gis_container * subcontainer[2][2];
	unsigned long latitudemask;
	unsigned long longitudemask;
	unsigned int bitindex;
	bool usesubcontainer;
	double latitudemax;
	double longitudemax;
	double latitudemin;
	double longitudemin;
};

