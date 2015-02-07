#pragma once

struct gis_node {
public:
	gis_node(unsigned int, double, double);
	gis_node(){};
	unsigned int id;
	double latitude;
	double longitude;
private:
};

