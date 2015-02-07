#include "gis_node.h"


gis_node::gis_node(unsigned int offset, double lat, double lon) {
	id = offset;
	latitude = lat;
	longitude = lon;
}
