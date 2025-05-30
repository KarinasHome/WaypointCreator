#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>

#define HRM_INV -1000
#define MAX_POLYGON_DEF 1000


#define HRM_U_SECTOR 100

#define HRM_U_JUNCTION_DIST 5.0
#define HRM_U_URBAN_DIST 25.0
#define HRM_U_ANGLE_VAR 2.5
#define HRM_U_STREET_DIST 25.0

#define HRM_SAR_SECTOR_BIG m_WaypointData.m_Sections
#define HRM_SAR_SECTOR_FINE m_WaypointData.m_Sections
//#define HRM_SAR_WAYPOINTS 200
#define HRM_SAR_URBAN_DIST 10 
#define HRM_SAR_FOREST_DIST 1
#define HRM_SAR_WATER_DIST 1 

//#define HRM_FLAT_SURFACE_DIST 200.0
//#define HRM_FLAT_SURFACE_ANGLE 5

//#define HRM_STEEP_SURFACE_ANGLE_MIN 25
//#define HRM_STEEP_SURFACE_ANGLE_MAX 65

#define HRM_STREET_TYPE_MIN 20
#define HRM_STREET_TYPE_MAX 80

const int dem_width = 2001;
const int dem_height = 2001;

const bool validation_file = true;

extern std::recursive_mutex outputListSharedMutex;

namespace WaypointCreator
{
	const int LatMin = -90;
	const int LatMax = 90;
	const int LonMin = -180;
	const int LonMax = 180;
}

enum PolygonDef
{
	Invalid,
	Object,
	Facade,
	Forest,
	Beach,
	Net,
	Lin,
	Pol,
	String,
	AutogenBlock,
	AutogenString,
	Water
};


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

struct vertex_points
{
	point p[3];
	bool vertex_valid = false;
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

struct sar_field_big
{
	bool has_terrain = false;
	bool is_usable = true;
	bool is_forest = false;
	bool is_water = false;
	bool is_urban = false;

	double latitude = HRM_INV;
	double logitude = HRM_INV;

	float elevation = 0;
};

struct polygon_winding
{
	std::vector<point> polygon_points;
};

struct dsf_polygon
{
	std::vector<polygon_winding> polygon_windings;
};

inline void writeOutput(std::string outputTextIn, CListBox& outputListIn)
{
	//std::unique_lock<std::recursive_mutex> physics_shm_lock(outputListSharedMutex, std::defer_lock);
	outputListIn.AddString(CA2CT(outputTextIn.c_str()));
	outputListIn.SetTopIndex(outputListIn.GetCount() - 1);
}

inline std::string getTime()
{
	time_t now = time(NULL);
	struct tm* tm_struct = localtime(&now);
	std::string hour = std::to_string(tm_struct->tm_hour);
	std::string min = std::to_string(tm_struct->tm_min);
	if (hour.size() <= 1) hour = "0" + hour;
	if (min.size() <= 1)  min = "0" + min;
	std::string ret =  hour + ":" + min;
	return ret;
}

inline int GetUrbanSegment(double latitude, double longitude)
{


	double delta_lat = abs(latitude - ((int)latitude));
	double delta_long = abs(longitude - ((int)longitude));

	int index_lat = (int)(min(delta_lat / (1.0 / ((double)HRM_U_SECTOR)), ((double)HRM_U_SECTOR) - 1.0));
	int index_long = (int)(min(delta_long / (1.0 / ((double)HRM_U_SECTOR)), ((double)HRM_U_SECTOR) - 1.0));

	return (index_lat * HRM_U_SECTOR) + index_long;
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




