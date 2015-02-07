#include "gis_segment.h"


gis_segment::gis_segment(unsigned int edge, double lat1, double lon1, double lat2, double lon2) {
	edgeid = edge;
	latitude1 = lat1;
	longitude1 = lon1;
	latitude2 = lat2;
	longitude2 = lon2;
}

gis_segment::~gis_segment(void) {
}
