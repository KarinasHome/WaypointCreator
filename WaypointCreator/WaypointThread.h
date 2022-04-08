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

	bool m_TerrainDataFound = false;

	PolygonDef m_PolygonDefinitions[MAX_POLYGON_DEF];
	urban_field *mp_UrbanField;
	std::vector<street_junction> m_ConsideredStreetJunctions;


public:
	WaypointThread(WaypointCreationData waypointDataIn, CListBox& outputList, CProgressCtrl& progressBar);
	~WaypointThread();

	void DoEvents(void);
	void RunComputation();
	void AnalyzeFile(std::string filename);
	void AnalyzeStreetWaypoints(std::string fms_filename);
	void AnalyzeUrbanWaypoints(std::string fms_filename);
	void CheckStreetWaypoint(double lat1, double long1, double lat2, double long2, int sub_type);
	double calc_distance_m(double lat1, double long1, double lat2, double long2);

};

