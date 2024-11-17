#pragma once

#include "common.h"
#include <vector>
#include <list>
#include <algorithm>
#include "TSQueue.h"
//#include "WaypointCreatorDlg.h"

struct WaypointCreationData
{
	int m_Lat = 0;
	int m_Lon = 0;

	bool m_StreetMissions = false;
	bool m_UrbanMissions = false;
	bool m_SARMissions = false;
	bool m_SlingMissions = false;
	bool m_do_not_recompute = false;
	int m_Sections = 100;
	int m_WaypointsMax = 400;

	float m_SARFlatSurfaceSlope = 5;
	float m_SARFlatSurfaceMinSlope = 0.1f;
	float m_SlingMinSlope = 25;
	float m_SlingMaxSlope = 65;

	float m_JunctionDist = 5.0;
	float m_UrbanDist = 25;
	float m_AngleVariation = 2.5;
	float m_StreetDist = 25;

	float m_SARUrbanDist = 5;
	float m_SARWaterDist = 3;
	float m_SARFlatSurfaceDist = 200;
	float m_SARMinAlt = 1500;

	std::string m_XP11_Path = "";
	std::string m_tmp_path = "";
	std::list<std::string> m_SceneryPathList;

	CDialogEx *m_dialog = NULL;
};

class WaypointThread
{

	WaypointCreationData m_WaypointData;
	std::vector<waypoint*> m_StreetWaypointVector;
	std::vector<dsf_polygon> m_ForestVector;
	std::vector<dsf_polygon> m_BeachVector;
	std::vector<dsf_polygon> m_UrbanVector;
	std::vector<polygon_winding> m_WaterVector;
	std::vector<point> m_StreetPoints;
	std::vector<polygon_winding> m_ForestExclusionVector;
	std::vector<polygon_winding> m_StreetExclusionVector;
	//CListBox& m_OutputList;
	//CProgressCtrl& m_ProgressBar;

	double m_MPerLat = HRM_INV;
	double m_MPerLon = HRM_INV;
	double m_LatPerM = HRM_INV;
	double m_LonPerM = HRM_INV;

	int m_MinDist = 100;
	//int m_MaxWaypoints = 500;
	int m_WIndex = 1;

	int m_DemWidth = 2001;
	int m_DemHeight = 2001;

	int m_elev_width = 1201;
	int m_elev_height = 1201;
	int m_elev_bpp = 2;
	float m_elev_scale = 1.0f;
	float m_elev_offset = 0;
	int m_elev_version = 0;
	std::string m_elev_filename = "";
	std::string m_thread_name = "";
	std::string m_thread_prefix = "";

	bool m_ElevationFileFound = false;

	bool m_TerrainDataFound = false;
	sar_field_big* mp_sar = NULL;
	std::vector<point> m_water_points;

	PolygonDef m_PolygonDefinitions[MAX_POLYGON_DEF];
	urban_field *mp_UrbanField;
	std::vector<street_junction> m_ConsideredStreetJunctions;


public:
	TSQueue<std::string> m_output_messages;
	std::atomic<int> m_progress = 0;
	std::atomic<int> m_running = true;
	std::atomic<int> m_stop = false;


	WaypointThread(WaypointCreationData waypointDataIn);
	~WaypointThread();

	void DoEvents(void);
	void RunComputation(int index);
	bool getFlatPoint(std::vector<dsf_polygon>& forest_vector, sar_field_big* p_sar, int width, int height, double lat_origin, double long_origin, int act_x, int act_y, int delta_x, int delta_y, waypoint& w_out, int index, bool is_slingload, bool is_water);
	void AnalyzeFile(std::string filename, std::string workpath);
	void AnalyzeStreetWaypoints(std::string fms_filename);
	void AnalyzeUrbanWaypoints(std::string fms_filename);
	void AnalyzeSARWaypoints(std::string fms_filename, bool sling_load, bool find_water = false);
	void CheckStreetWaypoint(double lat1, double long1, double lat2, double long2, int sub_type);
	void CheckStop();

};

