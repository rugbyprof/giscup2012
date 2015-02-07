#include <vector>
#include "gis_segment.h"
#include "gis_container.h"

using namespace std;

/**
 * Helper function to determine the points at which a target latitude and longitude
 * are intercepted by a line defined by two points, (lat1, lon1) and (lat2, lon2).
 * TODO: Verify formulas and assumption that lat = y and lon = x.
 * Target result is that for a given segment, intercept points will be found at
 * (targetlatN, loninterceptN) and at (latinterceptN, targetlonN), giving a total
 * of six points of intersection.
 */
void get_multiple_latlon_intercept(double &latintercept1, double &latintercept2, double &latintercept3, double &lonintercept1, double &lonintercept2, double &lonintercept3, double lat1, double lon1, double lat2, double lon2, double targetlat1, double targetlat2, double targetlat3, double targetlon1, double targetlon2, double targetlon3) {
	double m, b;
	m = (lat2 - lat1) / (lon2 - lon1);
	b = lat1 - (m * lon1);
	lonintercept1 = (targetlat1 - b) / m;
	lonintercept2 = (targetlat2 - b) / m;
	lonintercept3 = (targetlat3 - b) / m;
	latintercept1 = (m * targetlon1) + b;
	latintercept2 = (m * targetlon2) + b;
	latintercept3 = (m * targetlon3) + b;
}

/**
 * Helper function to determine whether or not a segment defined by endpoints
 * (edgelat1, edgelon1) and (edgelat2, edgelon2), and its accompanying
 * intercept points are within the bounds of the box described by
 * (latmin, lonmin), (latmax, lonmax).
 * The 3 intercept points to check are as follows:
 *   (latmin, lonintercept1)
 *   (latintercept1, lonmin)
 *   (latintercept2, lonmax)
 * The 4th intercept point that exists (latmax, lonintercept2) is not used,
 * because the results can be computed without including it in the stack.
 */
inline bool intercepts_within_range(double edgelat1, double edgelon1, double edgelat2, double edgelon2, double latmin, double lonmin, double latmax, double lonmax, double latintercept1, double lonintercept1, double latintercept2) {
	// If one end of the segment is within the bounds of the box, then
	// include it automatically.
	// NOTE: For logic later in the function, this is the only
	// possible scenario in which only one of the provided intercepts
	// might be within the bounds of the box.
	if (((latmin <= edgelat1) && (edgelat1 < latmax) && (lonmin <= edgelon1) && (edgelon1 < lonmax))
		|| ((latmin <= edgelat2) && (edgelat2 < latmax) && (lonmin <= edgelon2) && (edgelon2 < lonmax))) {
		return true;
	}

	// If both ends of the segment are outside of the same side of the bounds
	// of the box, then reject it automatically.
	// NOTE: For logic later in the function, this is the only possible
	// scenario in which two of the provided intercepts could be within the
	// bounds of the box, and yet the segment does not intersect with the box.
	if (((edgelat1 >= latmax) && (edgelat2 >= latmax))
		|| ((edgelat1 < latmin) && (edgelat2 < latmin))
		|| ((edgelon1 >= lonmax) && (edgelon2 >= lonmax))
		|| ((edgelon1 < lonmin) && (edgelon2 < lonmin))) {
		return false;
	}

	// Final case.  At this point, logic dictates that the segment does not
	// terminate within the box, neither are its endpoints solely above,
	// below, to the left, or to the right, of the box.  Because we eliminated
	// the preceding cases, however, we have one logical surety: that if any two
	// box intercepts are within the bounds of the box, then the segment passes
	// through the box, and therefore should be included.  Also, because all
	// segments are straight, either exactly two intercepts will be within the
	// bounds of the box, or none at all.  Therefore, for speed of processing,
	// only 3 of the four intercepts need to be tested.  If the first 3 fail,
	// then it is impossible for the fourth to pass.
	if (((latmin <= latintercept1) && (latintercept1 < latmax))
		|| ((latmin <= latintercept2) && (latintercept2 < latmax))
		|| ((lonmin <= lonintercept1) && (lonintercept1 < lonmax))) {
		return true;
	}

	// All logical possibilities are exhausted.
	return false;
}

gis_container::gis_container(void) {
	mysegmentids = 0;
	subcontainer[0][0] = subcontainer[0][1] = subcontainer[1][0] = subcontainer[1][1] = 0;
	usesubcontainer = false;
}

void gis_container::setdata(unsigned long latitudemask, unsigned long longitudemask, unsigned int bitindex, double latitudemax, double longitudemax, double latitudemin, double longitudemin) {
	gis_container::latitudemask = latitudemask;
	gis_container::longitudemask = longitudemask;
	gis_container::bitindex = bitindex;
	gis_container::latitudemax = latitudemax;
	gis_container::longitudemax = longitudemax;
	gis_container::latitudemin = latitudemin;
	gis_container::longitudemin = longitudemin;
}

gis_container::~gis_container(void) {
	if (mysegmentids != 0) {
		delete mysegmentids;
	}
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (subcontainer[i][j] != 0) {
				delete subcontainer[i][j];
			}
		}
	}
}

/**
 * Compute a boolean[4] array that indicates which quadrant(s) the segment
 * should be inserted into. Quadrants numbered thusly:
 *
 * |---|---|
 * | 2 | 3 |
 * |---|---|
 * | 0 | 1 |
 * |---|---|
 * 
 * When considered in binary, these values match the bitmask latitude and
 * longitude bits in the following formula: (latitude << 1) + longitude
 */
void gis_container::get_quadrants(bool quadrant[4], double lat1, double lon1, double lat2, double lon2) {
	double latmid, lonmid;
	// Clear the quadrants
	quadrant[0] = quadrant[1] = quadrant[2] = quadrant[3] = false;

	// Pre-compute variables to minimize duplication
	latmid = latitudemax - ((latitudemax - latitudemin) / 2);
	lonmid = longitudemax - ((longitudemax - longitudemin) / 2);
	bool lat1gtmid, lon1gtmid, lat2gtmid, lon2gtmid;
	lat1gtmid = lat1 > latmid;
	lon1gtmid = lon1 > lonmid;
	lat2gtmid = lat2 > latmid;
	lon2gtmid = lon2 > lonmid;
	if ((lat1gtmid == lat2gtmid) && (lon1gtmid == lon2gtmid)) {
		// The segment is located exclusively in a single quadrant.
		quadrant[((int)lat1gtmid << 1) + (int)lon1gtmid] = true;
	}
	else {
		// It is now known that the segment endpoints reside in different
		// quadrants, although we do not know the relation of these points
		// to the subcontainers' bounding box.
		if (lat1 == lat2) {
			// Segment is straight east and west.
			if (lat1gtmid) {
				// Segment is horizontal, in quadrants 2 and 3
				quadrant[2] = true;
				quadrant[3] = true;
			}
			else {
				// Segment is horizontal, in quadrants 0 and 1
				quadrant[0] = true;
				quadrant[1] = true;
			}
		}
		else if (lon1 == lon2) {
			// Segment is straight north and south.
			if (lon1gtmid) {
				// Segment is vertical, in quadrants 1 and 3
				quadrant[1] = true;
				quadrant[3] = true;
			}
			else {
				// Segment is vertical, in quadrants 0 and 2
				quadrant[0] = true;
				quadrant[2] = true;
			}
		}
		else {
			// Worst case scenario (although probably most common).
			// Segments cannot be tested empirically as to which quadrant they
			// are in. Determine the latitude and longitude interceps for the
			// possible quadrant boundaries.
			double latintercept1, latintercept2, latintercept3, lonintercept1, lonintercept2, lonintercept3;
			// Complicated function to reduce duplicated * and / operations.
			// Results are paired as such:
			//   latintercept1, longitudemin
			//   latintercept2, lonmid
			//   latintercept3, longitudemax
			//   latitudemin,   lonintercept1
			//   latmid,        lonintercept2
			//   latitudemax,   lonintercept3
			get_multiple_latlon_intercept(
				latintercept1, latintercept2, latintercept3,
				lonintercept1, lonintercept2, lonintercept3,
				lat1, lon1, lat2, lon2, // ends of the line segment
				latitudemin, latmid, latitudemax,
				longitudemin, lonmid, longitudemax);
			if (intercepts_within_range(lat1, lon1, lat2, lon2, latitudemin, longitudemin, latmid, lonmid, latintercept1, lonintercept1, latintercept2)) {
				quadrant[0] = true;
			}
			if (intercepts_within_range(lat1, lon1, lat2, lon2, latitudemin, lonmid, latmid, longitudemax, latintercept2, lonintercept1, latintercept3)) {
				quadrant[1] = true;
			}
			if (intercepts_within_range(lat1, lon1, lat2, lon2, latmid, longitudemin, latitudemax, lonmid, latintercept1, lonintercept2, latintercept2)) {
				quadrant[2] = true;
			}
			if (intercepts_within_range(lat1, lon1, lat2, lon2, latmid, lonmid, latitudemax, longitudemax, latintercept2, lonintercept2, latintercept3)) {
				quadrant[3] = true;
			}
		}
	}
}

void gis_container::add_segment(vector<gis_segment> * segment, unsigned int segmentid) {
	int size;
	if (usesubcontainer == false) {
		// Try to insert the segment
		if (mysegmentids == 0) {
			// No segments have been inserted, so create the vector
			mysegmentids = new vector<unsigned int>(0);
		}
		size = mysegmentids->size();
		if (size >= GIS_CONTAINER_SEGMENT_THRESHOLD) {
			// Switch the flag so that this process may be called recursively
			usesubcontainer = true;
			for (int i = 0; i < size; i++) {
				add_segment(segment, (*mysegmentids)[i]);
			}
			// Also add the new segment recursively
			add_segment(segment, segmentid);
			// Cleanup
			delete mysegmentids;
			mysegmentids = 0;
		}
		else {
			// Simply increase the size of the vector and add the entry to the end
			mysegmentids->resize(size + 1);
			(*mysegmentids)[size] = segmentid;
		}
	}
	else {
		// Add the segment to all of the appropriate subcontainers
		bool quadrant[4];
		get_quadrants(quadrant, (*segment)[segmentid].latitude1, (*segment)[segmentid].longitude1, (*segment)[segmentid].latitude2, (*segment)[segmentid].longitude2); 
		for (int lat = 0; lat < 2; lat++) {
			for (int lon = 0; lon < 2; lon++) {
				if (quadrant[(lat << 1) + lon]) {
					if (subcontainer[lat][lon] == 0) {
						double latmax, latmin, lonmax, lonmin;
						double range;
						range = (latitudemax - latitudemin) / 2;
						if (lat == 0) {
							latmax = latitudemax - range;
							latmin = latitudemin;
						}
						else {
							latmax = latitudemax;
							latmin = latitudemax - range;
						}
						range = (longitudemax - longitudemin) / 2;
						if (lon == 0) {
							lonmax = longitudemax - range;
							lonmin = longitudemin;
						}
						else {
							lonmax = longitudemax;
							lonmin = longitudemax - range;
						}
						subcontainer[lat][lon] = new gis_container();
						subcontainer[lat][lon]->setdata(latitudemask >> 1, longitudemask >> 1, bitindex - 1, latmax, lonmax, latmin, lonmin);
					}
					subcontainer[lat][lon]->add_segment(segment, segmentid);
				}
			}
		}
	}
}
