#pragma once

struct gis_segment {
public:
	gis_segment(unsigned int edge, double lat1, double lon1, double lat2, double lon2);
	gis_segment(){};
	~gis_segment(void);
	unsigned int edgeid;
	double latitude1;
	double longitude1;
	double latitude2;
	double longitude2;
};
