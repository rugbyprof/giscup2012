#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <time.h>

#include "gis_map.h"
#include "gis_node.h"
#include "gis_segment.h"

using namespace std;

void parse_nodes(char * directory, vector<gis_node> &node) {
	string filename;
	unsigned int index, id;
	double latitude, longitude;
	filename = directory;
	filename += "\\WA_Nodes.txt";
	ifstream nodefile(filename);
	index = 0;
	while (nodefile >> id >> latitude >> longitude) {
		if (index >= node.size()) {
			node.resize(index + 1);
		}
		node[index].id = id;
		node[index].latitude = latitude;
		node[index].longitude = longitude;
		index++;
	}
	nodefile.close();
}

unsigned long parse_edge_geometry(char * directory, vector<gis_segment> &segment) {
	string filename, line = "";
	stringstream s;
	unsigned int index, edgeid;
	double lat1, lon1, lat2, lon2;
	size_t begin, end;
	bool first;
	char trash;

	filename = directory;
	filename += "\\WA_EdgeGeometry.txt";
	ifstream segmentfile(filename);
	index = 0;

	while (getline(segmentfile, line)) {
		// get the edge id
		edgeid = atol((char*)line.c_str());
		end = 0;
		for (int i = 0; i < 4; i++) {
			begin = end;
			end = line.find("^", begin + 1);
		}
		s.clear();
		s << line.substr(end + 1, line.size());
		first = true;
		while (s >> lat2) {
			s >> trash;
			s >> lon2;
			if (first) {
				first = false;
			}
			else {
				if (index >= segment.size()) {
					segment.resize(index + 1);
				}
				segment[index].edgeid = edgeid;
				segment[index].latitude1 = lat1;
				segment[index].longitude1 = lon1;
				segment[index].latitude2 = lat2;
				segment[index].longitude2 = lon2;

				index++;
			}
			s >> trash;
			lat1 = lat2;
			lon1 = lon2;
		}
	}
	segmentfile.close();
	return index;
}

unsigned long parse_edge_geometry2(char * directory, vector<gis_segment> &segment) {
	string filename, line = "";
	unsigned int index, edgeid;
	double lat1, lon1, lat2, lon2;
	size_t begin, end;
	bool first;

	filename = directory;
	filename += "\\WA_EdgeGeometry.txt";
	ifstream segmentfile(filename);
	index = 0;

	while (getline(segmentfile, line)) {
		// get the edge id
		edgeid = atol((char*)line.c_str());
		end = 0;
		for (int i = 0; i < 4; i++) {
			begin = end;
			end = line.find("^", begin + 1);
		}
		line = line.substr(end + 1, line.size());
		first = true;
		while ((end = line.find("^")) != -1) {
			lat2 = atof(line.substr(0, end).c_str());
			line = line.substr(end + 1, line.size());
			end = line.find("^");
			lon2 = atof(line.substr(0, end).c_str());
			line = line.substr(end + 1, line.size());
			if (first) {
				first = false;
			}
			else {
				if (index >= segment.size()) {
					segment.resize(index + 1);
				}
				segment[index].edgeid = edgeid;
				segment[index].latitude1 = lat1;
				segment[index].longitude1 = lon1;
				segment[index].latitude2 = lat2;
				segment[index].longitude2 = lon2;

				index++;
			}
			lat1 = lat2;
			lon1 = lon2;
		}
	}
	segmentfile.close();
	return index;
}

int copy_until(const char * haystack, char needle, char * target, int start) {
	int index = start, i = 0;
	while (haystack[index] != needle && haystack[index] != 0) {
		target[i] = haystack[index];
		i++;
		index++;
	}
	target[i] = 0;
	if (index == start) {
		return -1;
	}
	if (haystack[index] == needle) {
		i++;
	}
	return i;
}

unsigned long parse_edge_geometry3(char * directory, vector<gis_segment> &segment) {
	string filename;
	unsigned int index, edgeid;
	double lat1, lon1, lat2, lon2;
	int begin, count;
	bool first;
	char *line, string1[25];
	line = new char[1048576];

	filename = directory;
	filename += "\\WA_EdgeGeometry.txt";
	ifstream segmentfile(filename);
	index = 0;

	while (segmentfile.getline(line, 1048576)) {
		// get the edge id
		edgeid = atol(line);
		int j = 0;
		for (int i = 0; i < 4;) {
			if (line[j] == '^') {
				i++;
				begin = j;
			}
			j++;
		}
		first = true;
		while ((count = copy_until(line, '^', string1, begin + 1)) != -1) {
			begin += count;
			lat2 = atof(string1);
			count = copy_until(line, '^', string1, begin + 1);
			begin += count;
			lon2 = atof(string1);
			if (first) {
				first = false;
			}
			else {
				if (index >= segment.size()) {
					segment.resize(index + 1);
				}
				segment[index].edgeid = edgeid;
				segment[index].latitude1 = lat1;
				segment[index].longitude1 = lon1;
				segment[index].latitude2 = lat2;
				segment[index].longitude2 = lon2;

				index++;
			}
			lat1 = lat2;
			lon1 = lon2;
		}
	}
	delete [] line;
	segmentfile.close();
	return index;
}

int main(int argc, char *argv[]) {

	unsigned long count;
	clock_t begin, end;

	/*
	begin = clock();
	cout << "Parsing Nodes" << endl;
	vector<gis_node> node(0);
	parse_nodes(argv[1], node);
	cout << "Nodes Parsed" << endl;
	end = clock();
	cout << setprecision(15) << (double(end - begin) / CLOCKS_PER_SEC) << endl;;

	begin = clock();
	cout << "Parsing Segments" << endl;
	vector<gis_segment> segment(0);
	count = parse_edge_geometry(argv[1], segment);
	cout << "Segments Parsed" << endl;
	cout << "Total segments: " << count << endl;
	end = clock();
	cout << setprecision(15) << (double(end - begin) / CLOCKS_PER_SEC) << endl;;

	begin = clock();
	cout << "Parsing Segments" << endl;
	vector<gis_segment> segment2(0);
	count = parse_edge_geometry2(argv[1], segment2);
	cout << "Segments Parsed" << endl;
	cout << "Total segments: " << count << endl;
	end = clock();
	cout << setprecision(15) << (double(end - begin) / CLOCKS_PER_SEC) << endl;;
	*/
	begin = clock();
	cout << "Parsing Segments" << endl;
	vector<gis_segment> segment3(0);
	count = parse_edge_geometry3(argv[1], segment3);
	cout << "Segments Parsed" << endl;
	cout << "Total segments: " << count << endl;
	end = clock();
	cout << setprecision(15) << (double(end - begin) / CLOCKS_PER_SEC) << endl;;

	gis_map map;

	for (unsigned int i = 0; i < segment3.size(); i++) {
		map.add_segment(segment3[i].edgeid, segment3[i].latitude1, segment3[i].longitude1, segment3[i].latitude2, segment3[i].longitude2);
	}

	system("pause");
	return 0;
}
