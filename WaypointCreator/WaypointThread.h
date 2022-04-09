#pragma once

#include "common.h"
#include <vector>
#include <list>
#include <algorithm>
//#include "WaypointCreatorDlg.h"

struct WaypointCreationData
{
	int m_Lat = 0;
	int m_Lon = 0;

	bool m_StreetMissions = false;
	bool m_UrbanMissions = false;
	bool m_SARMissions = false;
	bool m_SlingMissions = false;
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

	std::string m_XP11_Path = "";
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
	std::vector<dsf_polygon> m_WaterVector;
	CListBox& m_OutputList;
	CProgressCtrl& m_ProgressBar;

	double m_MPerLat = HRM_INV;
	double m_MPerLon = HRM_INV;
	double m_LatPerM = HRM_INV;
	double m_LonPerM = HRM_INV;

	int m_MinDist = 100;
	int m_MaxWaypoints = 500;
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

	bool m_ElevationFileFound = false;

	bool m_TerrainDataFound = false;
	sar_field_big* mp_sar;
	std::vector<point> m_water_points;

	PolygonDef m_PolygonDefinitions[MAX_POLYGON_DEF];
	urban_field *mp_UrbanField;
	std::vector<street_junction> m_ConsideredStreetJunctions;


public:
	WaypointThread(WaypointCreationData waypointDataIn, CListBox& outputList, CProgressCtrl& progressBar);
	~WaypointThread();

	void DoEvents(void);
	void RunComputation();
	void AnalyzeFile(std::string filename, std::string workpath);
	void AnalyzeStreetWaypoints(std::string fms_filename);
	void AnalyzeUrbanWaypoints(std::string fms_filename);
	void AnalyzeSARWaypoints(std::string fms_filename, bool sling_load);
	void CheckStreetWaypoint(double lat1, double long1, double lat2, double long2, int sub_type);

};

