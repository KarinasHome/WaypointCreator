#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <algorithm>

#define HRM_INV -1000

extern std::recursive_mutex outputListSharedMutex;

namespace WaypointCreator
{
	const int LatMin = -90;
	const int LatMax = 90;
	const int LonMin = -180;
	const int LonMax = 180;
}

struct waypoint
{
	std::string name = "";
	double longitude = HRM_INV;
	double latitude = HRM_INV;
	double longitude_head = HRM_INV;
	double latitude_head = HRM_INV;
	double longitude_head2 = HRM_INV;
	double latitude_head2 = HRM_INV;
};

struct point
{
	double latitutde = HRM_INV;
	double longitude = HRM_INV;
};

struct street_section
{
	double latitutde1 = HRM_INV;
	double longitude1 = HRM_INV;
	double latitutde2 = HRM_INV;
	double longitude2 = HRM_INV;
};

struct street_junction
{
	double latitutdec = HRM_INV;
	double longitudec = HRM_INV;
	double latitutde2 = HRM_INV;
	double longitude2 = HRM_INV;
	double latitutde3 = HRM_INV;
	double longitude3 = HRM_INV;
};

struct urban_field
{
	std::vector<point> urban_polygon_points;
	std::vector<street_section> urban_street_sections;
	std::vector<street_junction> urban_street_junctions;
};

inline void writeOutput(std::string outputTextIn, CListBox& outputListIn)
{
	//std::unique_lock<std::recursive_mutex> physics_shm_lock(outputListSharedMutex, std::defer_lock);
	outputListIn.AddString(CA2CT(outputTextIn.c_str()));
}

inline bool exists_test(const std::string& name) {
	//struct stat buffer;
	//return (stat(name.c_str(), &buffer) == 0);

	bool file_exists = false;

	std::ifstream test(name);

	file_exists = test.is_open();
	test.close();

	return file_exists;
}

inline bool str_is_number(const char* pstr)
{
	bool is_number = true;

	if (pstr[0] == 0) return false;

	if (pstr[0] == '-') pstr++;

	while (pstr[0] != 0)
	{
		if (isdigit(pstr[0]) == 0)
			is_number = false;
		pstr++;
	}

	return is_number;

}


inline std::string ltrim(const std::string& s)
{
	std::string WHITESPACE = " \n\r\t\f\v";
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}


