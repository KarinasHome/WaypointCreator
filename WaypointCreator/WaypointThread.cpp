#include "pch.h"
#include "WaypointThread.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

WaypointThread::WaypointThread(WaypointCreationData waypointDataIn, CListBox &outputList) :
	m_OutputList(outputList)

{
	m_WaypointData = waypointDataIn;
}

void WaypointThread::RunComputation()
{
	int index_lat = m_WaypointData.m_Lat;
	int index_lon = m_WaypointData.m_Lon;

	std::string hrm_path = m_WaypointData.m_XP11_Path + "Resources\\plugins\\HRM\\Ortho4XP\\";;

	for (auto path : m_WaypointData.m_SceneryPathList)
	{

		std::string dsf_filename = "";
		int short_lat = (index_lat / 10) * 10;
		int short_long = (index_lon / 10) * 10;

		if ((index_lat < 0) && ((index_lat % 10) != 0)) short_lat -= 10;
		if ((index_lon < 0) && ((index_lon % 10) != 0)) short_long -= 10;

		dsf_filename += short_lat < 0 ? "-" : "+";
		if ((short_lat < 10) && (short_lat > -10)) dsf_filename += "0";
		dsf_filename += std::to_string(abs(short_lat));

		dsf_filename += short_long < 0 ? "-" : "+";
		if ((short_long < 100) && (short_long > -100)) dsf_filename += "0";
		if ((short_long < 10) && (short_long > -10)) dsf_filename += "0";
		dsf_filename += std::to_string(abs(short_long));

		//dsf_filename += "\\";

		std::string waypoint_filename = "";

		waypoint_filename += index_lat < 0 ? "-" : "+";
		if ((index_lat < 10) && (index_lat > -10)) waypoint_filename += "0";
		waypoint_filename += std::to_string(abs(index_lat));

		waypoint_filename += index_lon < 0 ? "-" : "+";
		if ((index_lon < 100) && (index_lon > -100)) waypoint_filename += "0";
		if ((index_lon < 10) && (index_lon > -10)) waypoint_filename += "0";
		waypoint_filename += std::to_string(abs(index_lon));
		

		hrm_path = m_WaypointData.m_XP11_Path +  "Resources\\plugins\\HRM\\CustomScenery\\";
		std::string dsf_full_path = path + "\\Earth nav data\\" + dsf_filename + "\\" + waypoint_filename;
		std::string dsf_work_dir = path + "\\Earth nav data\\" + dsf_filename + "\\";
		std::string dsf_out_path = dsf_work_dir + "out\\" + waypoint_filename + ".dsf";

		if (exists_test(dsf_full_path + ".dsf") == true)
		{
			writeOutput("Found: " + dsf_full_path + ".dsf", m_OutputList);
			std::string command = "7z e \"" + dsf_full_path + ".dsf\" -o\"" + dsf_work_dir + "out\"";
			system(command.c_str());

			if (exists_test(dsf_out_path) == true)
			{
				command = "DSFTool --dsf2text \"" + dsf_work_dir + "out\\" + waypoint_filename + ".dsf\" \"" + dsf_work_dir + "out\\" + waypoint_filename + ".txt\"";
				system(command.c_str());

				AnalyzeFile("" + dsf_work_dir + "out\\" + waypoint_filename + ".txt");

			}
			else
			{
				command = "DSFTool --dsf2text \"" + dsf_full_path + ".dsf\" \"" + dsf_full_path + ".txt\"";
				system(command.c_str());

				AnalyzeFile(dsf_full_path + ".txt");
			}
			
		}

		
		

		

		//Nicht vergessen: Extrahiertes DSF File wieder löschen
	}
	std::string fms_waypoint_filename = "";

	fms_waypoint_filename += index_lat < 0 ? "-" : "+";
	if ((index_lat < 10) && (index_lat > -10)) fms_waypoint_filename += "0";
	fms_waypoint_filename += std::to_string(abs(index_lat));

	fms_waypoint_filename += index_lon < 0 ? "-" : "+";
	if ((index_lon < 100) && (index_lon > -100)) fms_waypoint_filename += "0";
	if ((index_lon < 10) && (index_lon > -10)) fms_waypoint_filename += "0";
	fms_waypoint_filename += std::to_string(abs(index_lon));

	if (m_WaypointData.m_StreetMissions == true) AnalyzeStreetWaypoints(hrm_path + "street_" + fms_waypoint_filename + ".fms");


}


void WaypointThread::AnalyzeStreetWaypoints(std::string fms_filename)
{
	

	if (m_StreetWaypointVector.size() > 0)
	{

		writeOutput("StreetWaypointGeneration: " + fms_filename, m_OutputList);

		std::ofstream fms_file;
		fms_file.open(fms_filename);

		if (fms_file.is_open())
		{
			fms_file << "I" << std::endl;
			fms_file << "1100 Version" << std::endl;
			fms_file << "CYCLE " << 1809 << std::endl;
			fms_file << "DEP " << std::endl;
			fms_file << "DES " << std::endl;
			fms_file << "NUMENR 3" << std::endl;

			fms_file.precision(9);

			int step_size = m_StreetWaypointVector.size() / m_MaxWaypoints;
			if (step_size <= 0) step_size = 1;

			for (int index = 0; index < m_StreetWaypointVector.size(); index += step_size)
			{
				waypoint* p_way = m_StreetWaypointVector[index];
				fms_file << "28 " << p_way->name << " DEP 25000.000000 " << p_way->latitude << " " << p_way->longitude << std::endl;
				p_way->name += "_HEAD";
				fms_file << "28 " << p_way->name << " DEP 25000.000000 " << p_way->latitude_head << " " << p_way->longitude_head << std::endl;
				//p_way->name += "_2";
				//fms_file << "28 " << p_way->name << " DEP 25000.000000 " << p_way->latitude_head2 << " " << p_way->longitude_head2 << std::endl;
			}

			fms_file.close();
		}
	}
	else
	{
		writeOutput("No Streets found: " + fms_filename, m_OutputList);
	}
}

double WaypointThread::calc_distance_m(double lat1, double long1, double lat2, double long2)
{
	lat1 = lat1 * M_PI / 180;
	long1 = long1 * M_PI / 180;
	lat2 = lat2 * M_PI / 180;
	long2 = long2 * M_PI / 180;

	double rEarth = 6372797;

	double dlat = lat2 - lat1;
	double dlong = long2 - long1;

	double x1 = sin(dlat / 2);
	double x2 = cos(lat1);
	double x3 = cos(lat2);
	double x4 = sin(dlong / 2);

	double x5 = x1 * x1;
	double x6 = x2 * x3 * x4 * x4;

	double temp1 = x5 + x6;

	double y1 = sqrt(temp1);
	double y2 = sqrt(1.0 - temp1);

	double temp2 = 2 * atan2(y1, y2);

	double range_m = temp2 * rEarth;

	return range_m;
}

void WaypointThread::CheckStreetWaypoint(double lat1, double long1, double lat2, double long2, int sub_type)
{
	if (lat1 == HRM_INV) return;
	if (lat2 == HRM_INV) return;
	if (long1 == HRM_INV) return;
	if (long2 == HRM_INV) return;

	if ((m_MPerLat == HRM_INV) || (m_MPerLon == HRM_INV))
	{
		m_MPerLat = abs(calc_distance_m(lat1, long1, lat1 + 1.0, long1));
		m_MPerLon = abs(calc_distance_m(lat1, long1, lat1, long1 + 1.0));
	}

	//if ((sub_type != 50) && (sub_type != 40) && (sub_type != 30) && (sub_type != 20)) return;
	if ((sub_type > 40) || (sub_type <= 20)) return;

	double delta_lat = abs(lat1 - lat2) * m_MPerLat;
	double delta_long = abs(long1 - long2) * m_MPerLon;

	if ((delta_lat > m_MinDist) || (delta_long > m_MinDist))
	{
		waypoint* p_way = new waypoint();

		p_way->name = "WP_" + std::to_string(sub_type) + "_" + std::to_string(m_WIndex++);
		p_way->latitude = ((lat1 + lat2) / 2.0);

		p_way->longitude = ((long1 + long2) / 2.0);

		p_way->latitude_head = lat1;
		p_way->longitude_head = long1;
		p_way->latitude_head2 = lat2;
		p_way->longitude_head2 = long2;

		m_StreetWaypointVector.push_back(p_way);
	}
}

void WaypointThread::AnalyzeFile(std::string filename)
{
	if (m_TerrainDataFound == true)
	{
		writeOutput("After Terrain -> Skipped: " + filename, m_OutputList);
		return;
	}


	writeOutput("Analyzing: " + filename, m_OutputList);

	std::ifstream dsf_file(filename);
	std::string line_string;

	for (int index = 0; index < MAX_POLYGON_DEF; index++)
	{
		m_PolygonDefinitions[index] = PolygonDef::Invalid;
	}

	sar_field_big* p_sar = NULL;


	double long_1 = HRM_INV;
	double lat_1 = HRM_INV;
	double long_2 = HRM_INV;
	double lat_2 = HRM_INV;

	bool is_water = false;


	std::vector<point> water_points;  // So far water points are local, should be members?
	//std::vector<waypoint> considered_points;

	//std::vector<int> water_defs;

	int elevation_pos = 0;
	bool elevation_pos_found = false;
	bool is_overlay = false;
	bool is_valid_terrain = false;
	int poly_index = 0;

	//bool elevation_file_found = false;
	int elevation_file_count = 0;

	int elev_width = 1201;
	int elev_height = 1201;
	int elev_bpp = 2;
	float elev_scale = 1.0f;
	float elev_offset = 0;
	int elev_version = 0;
	int terrain_def_count = 0;
	

	std::string elev_filename = "";


	while (std::getline(dsf_file, line_string))
	{
		std::stringstream line_stream(line_string);

		std::string item_1 = "";
		std::string item_2 = "";

		line_stream >> item_1;

		if (item_1.compare("PROPERTY") == 0)
		{
			line_stream >> item_2;

			if (item_2.find("overlay 1") != std::string::npos)
			{
				is_overlay = true;
			}
		}
		else if (item_1.compare("TERRAIN_DEF") == 0)
		{
			if ((is_overlay == false) && (m_TerrainDataFound == false))
			{
				m_TerrainDataFound = true;
				is_valid_terrain = true;
			}

			if (is_valid_terrain == true)
			{

				line_stream >> item_2;
				//if ((item_2.find("Water") != std::string::npos) || (item_2.find("water") != std::string::npos))
				//	water_defs.push_back(terrain_def_count);

				terrain_def_count++;
			}
		}

		else if (item_1.compare("RASTER_DEF") == 0)
		{
			if ((is_overlay == false) && (m_TerrainDataFound == false))
			{
				m_TerrainDataFound = true;
				is_valid_terrain = true;
			}

			if (is_valid_terrain == true)
			{
				line_stream >> item_2;

				if (item_2.compare("elevation") == 0)
				{
					elevation_pos_found = true;
				}
				else
				{
					if (elevation_pos_found == false) elevation_pos++;
				}
			}
		}
		/*else if (item_1.compare("RASTER_DATA") == 0)
		{
			if ((is_overlay == false) && (m_TerrainDataFound == false))
			{
				m_TerrainDataFound = true;
				is_valid_terrain = true;
			}

			if (is_valid_terrain == true)
			{
				if (elevation_file_count == elevation_pos)
				{
					line_stream >> item_2; // version
					line_stream >> item_2; // bpp
					sscanf_s(item_2.c_str(), "bpp=%i", &elev_bpp);

					line_stream >> item_2; // flags

					line_stream >> item_2; // width
					sscanf_s(item_2.c_str(), "width=%i", &elev_width);

					line_stream >> item_2; // height
					sscanf_s(item_2.c_str(), "height=%i", &elev_height);

					line_stream >> item_2; // scale
					sscanf_s(item_2.c_str(), "scale=%f", &elev_scale);

					line_stream >> item_2; // offset
					sscanf_s(item_2.c_str(), "offset=%f", &elev_offset);

					line_stream >> elev_filename;

				}
				elevation_file_count++;
			}
		}


		else if (item_1.compare("POLYGON_DEF") == 0)
		{
			if ((is_overlay == false) && (m_TerrainDataFound == false))
			{
				m_TerrainDataFound = true;
				is_valid_terrain = true;
			}

			if ((is_valid_terrain == true) || (is_overlay == true))
			{
				line_stream >> item_2;
				if (poly_index < MAX_POLYGON_DEF)
				{
					if (item_2.find(".obj") != std::string::npos)			  m_PolygonDefinitions[poly_index] = PolygonDef::Object;
					if (item_2.find(".fac") != std::string::npos)			  m_PolygonDefinitions[poly_index] = PolygonDef::Facade;
					if (item_2.find(".for") != std::string::npos)		   	  m_PolygonDefinitions[poly_index] = PolygonDef::Forest;
					if (item_2.find(".bch") != std::string::npos)	    	  m_PolygonDefinitions[poly_index] = PolygonDef::Beach;
					if (item_2.find(".net") != std::string::npos)    		  m_PolygonDefinitions[poly_index] = PolygonDef::Net;
					if (item_2.find(".lin") != std::string::npos)			  m_PolygonDefinitions[poly_index] = PolygonDef::Lin;
					if (item_2.find(".pol") != std::string::npos)			  m_PolygonDefinitions[poly_index] = PolygonDef::Pol;
					if (item_2.find(".str") != std::string::npos)			  m_PolygonDefinitions[poly_index] = PolygonDef::String;
					if (item_2.find(".ags") != std::string::npos)			  m_PolygonDefinitions[poly_index] = PolygonDef::AutogenString;
					if (item_2.find(".agb") != std::string::npos)			  m_PolygonDefinitions[poly_index] = PolygonDef::AutogenBlock;
					if (item_2.find("terrain_Water") != std::string::npos)    m_PolygonDefinitions[poly_index] = PolygonDef::Water;
				}
				poly_index++;
			}
		}
		else if (item_1.compare("BEGIN_POLYGON") == 0)
		{
			if ((is_overlay == false) && (m_TerrainDataFound == false))
			{
				m_TerrainDataFound = true;
				is_valid_terrain = true;
			}

			if ((is_valid_terrain == true) || (is_overlay == true))
			{
				bool end_segment = false;
				is_water = false;

				int poly_type = -1;
				line_stream >> poly_type;

				dsf_polygon current_polygon;
				polygon_winding current_winding;

				while ((end_segment == false))
				{
					std::getline(dsf_file, line_string);

					std::stringstream line_stream_2(line_string);

					std::string item_1 = "";
					line_stream_2 >> item_1;

					if (item_1.compare("BEGIN_WINDING") == 0)
					{
						current_winding.polygon_points.clear();
					}
					else if (item_1.compare("END_WINDING") == 0)
					{
						current_polygon.polygon_windings.push_back(current_winding);
					}
					else if (item_1.compare("POLYGON_POINT") == 0)
					{
						long_1 = HRM_INV;
						lat_1 = HRM_INV;

						line_stream_2 >> long_1;
						line_stream_2 >> lat_1;

						point current_point;
						current_point.latitutde = lat_1;
						current_point.longitude = long_1;

						current_winding.polygon_points.push_back(current_point);
					}

					else if (item_1.compare("END_POLYGON") == 0)
					{
						//***************************************************************************************************
						//ToDo: Add Exclusion Zone Check
						//***************************************************************************************************


						if ((poly_type >= 0) && (current_polygon.polygon_windings.size() > 0))
						{
							if (m_PolygonDefinitions[poly_type] == PolygonDef::Forest)			m_ForestVector.push_back(current_polygon);
							else if (m_PolygonDefinitions[poly_type] == PolygonDef::Beach)			m_BeachVector.push_back(current_polygon);
							else if (m_PolygonDefinitions[poly_type] == PolygonDef::Facade)			m_UrbanVector.push_back(current_polygon);
							else if (m_PolygonDefinitions[poly_type] == PolygonDef::AutogenBlock)	m_UrbanVector.push_back(current_polygon);
							else if (m_PolygonDefinitions[poly_type] == PolygonDef::AutogenString)	m_UrbanVector.push_back(current_polygon);
							else if (m_PolygonDefinitions[poly_type] == PolygonDef::Water)			m_WaterVector.push_back(current_polygon);
						}
					}
				}
			}

		}

		else if (item_1.compare("BEGIN_PATCH") == 0)
		{
			int terrain_type = 0;

			line_stream >> terrain_type;

			if (terrain_type == 0)
			{
				is_water = true;
			}
			else
			{
				/// ///////////////////////////////////////////////////////////////////////////////////////////////
				//
				//ToDo: Add For Textured Water (Ortho4XP)
				//****************************************************************************************************

				is_water = false;

				//for (auto t_number : m_WaterVector)
				//{
				//	if (t_number == terrain_type)
				//		is_water = true;
				//}
			}
		}

		else if (item_1.compare("END_PATCH") == 0)
		{
			is_water = false;
		}

		else if (item_1.compare("PATCH_VERTEX") == 0)
		{
			if ((is_overlay == false) && (m_TerrainDataFound == false))
			{
				m_TerrainDataFound = true;
				is_valid_terrain = true;
			}

			if (is_valid_terrain == true)
			{
				if (elev_filename.size() > 0)
				{
					if (is_water == true)
					{
						point p;
						line_stream >> p.longitude;
						line_stream >> p.latitutde;

						water_points.push_back(p);
					}
				}

				else
				{
					if (p_sar == NULL)
					{
						p_sar = new sar_field_big[dem_height * dem_width];
						elev_height = dem_height;
						elev_width = dem_width;

						for (int index = 0; index < (elev_height * elev_width); index++)
						{
							p_sar[index].is_usable = true;
						}
					}
					else
					{
						double latitude;
						double longitude;
						float elevation;

						line_stream >> longitude;
						line_stream >> latitude;
						line_stream >> elevation;

						int x = min((int)((longitude - m_WaypointData.m_Lon) * ((double)(elev_width - 1))), elev_width - 1);
						int y = min((int)((latitude - m_WaypointData.m_Lat) * ((double)(elev_height - 1))), elev_height - 1);

						p_sar[(y * elev_height) + x].is_usable = !is_water;
						p_sar[(y * elev_height) + x].elevation = elevation;

					}
				}

			}
		}

		else if (item_1.compare("BEGIN_SEGMENT") == 0)
		{

			int type = -1;
			int sub_type = -1;
			int node_id = -1;
			point p;

			line_stream >> type;
			line_stream >> sub_type;
			line_stream >> node_id;
			line_stream >> p.longitude;
			line_stream >> p.latitutde;

			water_points.push_back(p);
		}

		else if (item_1.compare("END_SEGMENT") == 0)
		{
			int node_id = -1;


			line_stream >> node_id;

			point prev = water_points[water_points.size() - 1];

			point p;
			line_stream >> p.longitude;
			line_stream >> p.latitutde;

			double delta_lat = p.latitutde - prev.latitutde;
			double delta_long = p.longitude - prev.longitude;

			for (double index = 0; index < 10.0; index++)
			{
				point p_delta;

				p_delta.latitutde = prev.latitutde + (delta_lat * index / 10.0);
				p_delta.longitude = prev.longitude + (delta_long * index / 10.0);

				water_points.push_back(p_delta);
			}

			water_points.push_back(p);
		}
		else if (item_1.compare("SHAPE_POINT") == 0)
		{
			point prev = water_points[water_points.size() - 1];




			point p;
			line_stream >> p.longitude;
			line_stream >> p.latitutde;

			double delta_lat = p.latitutde - prev.latitutde;
			double delta_long = p.longitude - prev.longitude;

			for (double index = 0; index < 10.0; index++)
			{
				point p_delta;

				p_delta.latitutde = prev.latitutde + (delta_lat * index / 10.0);
				p_delta.longitude = prev.longitude + (delta_long * index / 10.0);

				water_points.push_back(p_delta);
			}



			water_points.push_back(p);


		}*/

		else if (item_1.compare("BEGIN_SEGMENT") == 0)
		{

			int type = -1;
			int sub_type = -1;
			int node_id = -1;
			int elevation_1 = 0;
			int elevation_2 = 0;
			point p;
			line_stream >> type;
			line_stream >> sub_type;
			line_stream >> node_id;
			line_stream >> long_1;
			line_stream >> lat_1;
			line_stream >> elevation_1;


			bool end_segment = false;

			//water_points.push_back(p);

			if (sub_type > 0)
			{
				while ((end_segment == false))
				{
					std::getline(dsf_file, line_string);
					std::stringstream line_stream_2(line_string);

					std::string item_2 = "";
					line_stream_2 >> item_2;

					if (item_2.compare("SHAPE_POINT") == 0)
					{
						line_stream_2 >> long_2;
						line_stream_2 >> lat_2;
						line_stream_2 >> elevation_2;

						if ((elevation_1 == 0) && (elevation_2 == 0))
							CheckStreetWaypoint(lat_1, long_1, lat_2, long_2, sub_type);

						lat_1 = lat_2;
						long_1 = long_2;
						elevation_1 = elevation_2;

					}
					else if (item_2.compare("END_SEGMENT") == 0)
					{
						line_stream_2 >> node_id;

						line_stream_2 >> long_2;
						line_stream_2 >> lat_2;
						line_stream_2 >> elevation_2;


						if ((elevation_1 == 0) && (elevation_2 == 0)) 
							CheckStreetWaypoint(lat_1, long_1, lat_2, long_2, sub_type);

						end_segment = true;

					}
					else
					{
						end_segment = true;
					}

				}
			}

		}

	}

	dsf_file.close();

	if (is_overlay == false)
	{
		//m_TerrainDataFound = true;
	}

	/*

	double delta_lat = 1.0 / ((double)(elev_height - 1));
	double delta_long = 1.0 / ((double)(elev_width - 1));

	std::cout << "SAR file read finished. Forests: " << m_ForestVector.size() << std::endl;


	if (p_sar == NULL)
		p_sar = ReadElevation(elev_filename, elev_width, elev_height, elev_bpp, elev_scale, elev_offset);
	else InterpolateElevation(p_sar, elev_width, elev_height);



	for (auto p : water_points)
	{
		p.latitutde -= tile_lat;
		p.longitude -= tile_long;

		int x = min(p.longitude * (elev_width - 1), elev_width - 1);
		int y = min(p.latitutde * (elev_height - 1), elev_height - 1);

		int x_start = max(0, x - HRM_SAR_WATER_DIST);
		int x_stop = min(elev_width - 1, x + HRM_SAR_WATER_DIST);

		int y_start = max(0, y - HRM_SAR_WATER_DIST);
		int y_stop = min(elev_height - 1, y + HRM_SAR_WATER_DIST);

		for (x = x_start; x <= x_stop; x++)
			for (y = y_start; y <= y_stop; y++)
				p_sar[(y * elev_height) + x].is_usable = false;

	}

	for (auto poly_vector : m_UrbanVector)
	{
		for (auto poly : poly_vector.polygon_windings)
		{
			if (poly.polygon_points.size() > 2)
			{
				point pmax = poly.polygon_points[0];
				point pmin = poly.polygon_points[0];

				for (auto p : poly.polygon_points)
				{
					pmax.latitutde = max(pmax.latitutde, p.latitutde);
					pmax.longitude = max(pmax.longitude, p.longitude);
					pmin.latitutde = min(pmin.latitutde, p.latitutde);
					pmin.longitude = min(pmin.longitude, p.longitude);
				}

				int x_start = abs(pmin.longitude - tile_long) / delta_long;
				int x_stop = abs(pmax.longitude - tile_long) / delta_long;

				int y_start = abs(pmin.latitutde - tile_lat) / delta_lat;
				int y_stop = abs(pmax.latitutde - tile_lat) / delta_lat;

				x_start = max(x_start - HRM_SAR_URBAN_DIST, 0);
				y_start = max(y_start - HRM_SAR_URBAN_DIST, 0);

				x_stop = min(x_stop + HRM_SAR_URBAN_DIST, elev_width);
				y_stop = min(y_stop + HRM_SAR_URBAN_DIST, elev_height);

				for (int x = x_start; x < x_stop; x++)
				{
					for (int y = y_start; y < y_stop; y++)
					{
						p_sar[(y * elev_height) + x].is_usable = false;
					}

				}
			}
		}

	}

	if (is_slingload == false)

	{

		for (auto windings : m_ForestVector)
		{

			// Get Rectangular Border
			//for (auto current_winding : windings.polygon_windings)
			if (windings.polygon_windings.size() > 0)
			{
				polygon_winding& current_winding = windings.polygon_windings[0];
				//paint_poly(current_winding.polygon_points, p_sar, tile_lat, tile_long, elev_width, elev_height);

				point pmax = current_winding.polygon_points[0];
				point pmin = current_winding.polygon_points[0];

				for (auto p : current_winding.polygon_points)
				{
					pmax.latitutde = max(pmax.latitutde, p.latitutde);
					pmax.longitude = max(pmax.longitude, p.longitude);
					pmin.latitutde = min(pmin.latitutde, p.latitutde);
					pmin.longitude = min(pmin.longitude, p.longitude);
				}

				int x_start = abs(pmin.longitude - tile_long) / delta_long;
				int x_stop = abs(pmax.longitude - tile_long) / delta_long;

				int y_start = abs(pmin.latitutde - tile_lat) / delta_lat;
				int y_stop = abs(pmax.latitutde - tile_lat) / delta_lat;

				x_start = max(x_start - HRM_SAR_URBAN_DIST, 0);
				y_start = max(y_start - HRM_SAR_URBAN_DIST, 0);

				x_stop = min(x_stop + HRM_SAR_URBAN_DIST, elev_width);
				y_stop = min(y_stop + HRM_SAR_URBAN_DIST, elev_height);

				for (int x = x_start; x < x_stop; x++)
				{
					for (int y = y_start; y < y_stop; y++)
					{
						double f_lat = tile_lat + ((double)y) / ((double)(elev_height - 1));
						double f_long = tile_long + ((double)x) / ((double)(elev_width - 1));

						bool is_forest = false;

						// Add last point
						//current_winding.polygon_points.push_back(current_winding.polygon_points[0]);

						// Check if point is in forest
						if (pnpoly_cpp(current_winding.polygon_points, f_lat, f_long) > 0)
							is_forest = true;


						// Remove clearings
						if (is_forest == true)
						{
							for (int index = 1; index < windings.polygon_windings.size(); index++)
							{
								polygon_winding& next_winding = windings.polygon_windings[index];
								if (pnpoly_cpp(next_winding.polygon_points, f_lat, f_long) > 0)
									is_forest = !is_forest;
							}
						}

						if (is_forest == true)
							p_sar[(y * elev_height) + x].is_usable = false;

					}

				}


			}




		}
	}
	//write_bmp_usable(p_sar, elev_width, elev_height);


	write_bmp_usable(p_sar, elev_width, elev_height);

	int delta_h = (elev_height - 1) / HRM_SAR_SECTOR_BIG;
	int delta_w = (elev_width - 1) / HRM_SAR_SECTOR_BIG;

	for (int x = 0; x < HRM_SAR_SECTOR_BIG; x++)
	{
		for (int y = 0; y < HRM_SAR_SECTOR_BIG; y++)
		{
			waypoint w_act;
			if (getFlatPoint(m_ForestVector, p_sar, elev_width, elev_height, tile_lat, tile_long, x * delta_w, y * delta_h, delta_w, delta_h, w_act, 0, is_slingload))
			{

				considered_points.push_back(w_act);
			}


		}
	}



	/*for (int offset_lat = 0; offset_lat < HRM_SAR_SECTOR_BIG; offset_lat++)
	{
		for (int offset_long = 0; offset_long < HRM_SAR_SECTOR_BIG; offset_long++)
		{
			int segment = (offset_lat * HRM_SAR_SECTOR_BIG) + offset_long;

			if ((p_sar[segment].has_terrain == true) && (p_sar[segment].has_urban == false))
			{

				if ((p_sar[segment].latitude != HRM_INV) && (p_sar[segment].logitude != HRM_INV) && (p_sar[segment].steepness < 10000))
				{
					int result = 0;
					for (auto forest : m_ForestVector)
					{
						result += pnpoly_cpp(forest.polygon_points, p_sar[segment].latitude, p_sar[segment].logitude);

					}

					if (result == 0)
					{
						point current_point;
						current_point.latitutde = p_sar[segment].latitude;
						current_point.longitude = p_sar[segment].logitude;
						considered_points.push_back(current_point);
					}
					else
					{
						result++;
					}


				}
			}
		}

	}*/




	/*for (int offset_lat = 0; offset_lat < HRM_SAR_SECTOR_BIG; offset_lat++)
	{
		for (int offset_long = 0; offset_long < HRM_SAR_SECTOR_BIG; offset_long++)
		{
			int segment = (offset_lat * HRM_SAR_SECTOR_BIG) + offset_long;

			if ((p_sar[segment].has_terrain == true) && (p_sar[segment].has_urban == false))
			{
				for (	double current_latitude = tile_lat + (1.0 / (2 * HRM_SAR_SECTOR_FINE)) + (((double)offset_lat) / HRM_SAR_SECTOR_BIG);
						current_latitude < (tile_lat + (((double)(offset_lat + 1) / HRM_SAR_SECTOR_BIG)));
						current_latitude += (1.0 / HRM_SAR_SECTOR_FINE))
				{
					for (	double current_longitude = tile_long + (1.0 / (2 * HRM_SAR_SECTOR_FINE)) + (((double)offset_long) / HRM_SAR_SECTOR_BIG);
							current_longitude < (tile_long + (((double) (offset_long + 1)) / HRM_SAR_SECTOR_BIG));
							current_longitude += (1.0 / HRM_SAR_SECTOR_FINE))
					{
						int result = 0;
						for (auto forest : m_ForestVector)
						{
							result += pnpoly_cpp(forest.polygon_points, current_latitude, current_longitude);

						}

						if (result == 0)
						{
							point current_point;
							current_point.latitutde = current_latitude;
							current_point.longitude = current_longitude;
							considered_points.push_back(current_point);
						}
						else
						{
							result++;
						}


					}
				}
			}
		}

	}*/

	/*if (considered_points.size() > 0)
	{

		std::ofstream fms_file;
		fms_file.open(waypoint_filename);

		if (fms_file.is_open())
		{
			fms_file << "I" << std::endl;
			fms_file << "1100 Version" << std::endl;
			fms_file << "CYCLE " << 1809 << std::endl;
			fms_file << "DEP " << std::endl;
			fms_file << "DES " << std::endl;
			fms_file << "NUMENR 3" << std::endl;

			fms_file.precision(9);

			int step_size = considered_points.size() / m_MaxWaypoints;
			if (step_size <= 0) step_size = 1;

			for (int index = 0; index < considered_points.size(); index += step_size)
			{
				waypoint& p = considered_points[index];
				fms_file << "28 " << p.name << index << " DEP 30000.000000 " << p.latitude << " " << p.longitude << std::endl;
				fms_file << "28 " << p.name << "HEAD_" << index << " DEP 30000.000000 " << p.latitude_head << " " << p.longitude_head << std::endl;
			}

			fms_file.close();
		}
	}*/

	if (p_sar != NULL) delete[] p_sar;
}
