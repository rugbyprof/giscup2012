#include <vector>
#include "gis_map.h"

/**
 * The index value scales a floating point latitude or longitude into a 32-bit
 * unsigned integer, where the index value "0" is the smallest possible
 * latitude or longitude value (-90 and -180, respectively) and the index value
 * "4,294,967,295" (max value of a 32-bit unsigned integer) is the largest
 * possible value of latitude or longitude (+90 and +180, respectively).
 * Specific bit offsets are then used to partition a parent container into
 * four subcontainers using the following scheme:
 * (Suppose the bit for latitude is in x and the bit for longitude is in y.)
 *
 *   [x][y] is mapped as:
 *
 *   |-----------------|
 *   | [1][0] | [1][1] |
 *   |--------+--------|
 *   | [0][0] | [0][1] |
 *   |-----------------|
 *
 * This scheme works well because in actual latitude and longitude values,
 * where (0, 0) is considered to be the center of the map, the smallest 
 * (e.g., negative) values are located southwest of center, and the
 * largest (e.g., positive) values are located northeast of center.
 **/

unsigned long lat2index(double latitude) {
	// Scale latitude (-90 to +90) from 0 to 4,294,967,295 (32-bit unsigned)
	return (unsigned long)((latitude + 90) / 180 * 4294967295);
}

unsigned long lon2index(double longitude) {
	// Scale longitude (-180 to +180) from 0 to 4,294,967,295 (32-bit unsigned)
	return (unsigned long)((longitude + 180) / 360 * 4294967295);
}

gis_map::gis_map(void) {
	container.setdata(1 << 31, 1 << 31, 32, 90, 180, -90, -180);
}


gis_map::~gis_map(void) {
}

unsigned int gis_map::add_segment(unsigned int edgeid, double latitude1, double longitude1, double latitude2, double longitude2) {
	int delta = segment.size();
	segment.resize(delta + 1);
	segment[delta].edgeid = edgeid;
	segment[delta].latitude1 = latitude1;
	segment[delta].longitude1 = longitude1;
	segment[delta].latitude2 = latitude2;
	segment[delta].longitude2 = longitude2;

	container.add_segment(&segment, delta);
	return delta;
}
