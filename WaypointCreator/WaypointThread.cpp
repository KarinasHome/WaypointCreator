#include "pch.h"
#include "WaypointThread.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

void write_bmp_usable(sar_field_big* p_buffer, int width, int height)
{
	CImage myimage;

	myimage.Create(width, height, 32);


	for (int h = 0; h < (height); h++)
		for (int w = 0; w < (width); w++)
		{
			BYTE b = 0;
			if (p_buffer[(h * width) + w].is_usable == true)
				b = min(p_buffer[(h * width) + w].elevation / 10, 255);
			//(BYTE)(p_buffer[(h*elev_width) + w] / 10);
			myimage.SetPixelRGB(w, height - h - 1, b, b, b);
		}

	LPCTSTR str = "test_usable.bmp";

	myimage.Save(str);

}

void write_bmp_forest(sar_field_big* p_buffer, int width, int height)
{
	CImage myimage;

	myimage.Create(width, height, 32);


	for (int h = 0; h < (height); h++)
		for (int w = 0; w < (width); w++)
		{
			BYTE b = 0;
			if (p_buffer[(h * width) + w].is_forest == false)
				b = min(p_buffer[(h * width) + w].elevation / 10, 255);
			//(BYTE)(p_buffer[(h*elev_width) + w] / 10);
			myimage.SetPixelRGB(w, height - h - 1, b, b, b);
		}

	LPCTSTR str = "test_forest.bmp";

	myimage.Save(str);

}

void write_bmp_water(sar_field_big* p_buffer, int width, int height)
{
	CImage myimage;

	myimage.Create(width, height, 32);


	for (int h = 0; h < (height); h++)
		for (int w = 0; w < (width); w++)
		{
			BYTE b = 0;
			if (p_buffer[(h * width) + w].is_water == false)
				b = min(p_buffer[(h * width) + w].elevation / 10, 255);
			//(BYTE)(p_buffer[(h*elev_width) + w] / 10);
			myimage.SetPixelRGB(w, height - h - 1, b, b, b);
		}

	LPCTSTR str = "test_water.bmp";

	myimage.Save(str);

}

void write_bmp_urban(sar_field_big* p_buffer, int width, int height)
{
	CImage myimage;

	myimage.Create(width, height, 32);


	for (int h = 0; h < (height); h++)
		for (int w = 0; w < (width); w++)
		{
			BYTE b = 0;
			if (p_buffer[(h * width) + w].is_urban == false)
				b = min(p_buffer[(h * width) + w].elevation / 10, 255);
			//(BYTE)(p_buffer[(h*elev_width) + w] / 10);
			myimage.SetPixelRGB(w, height - h - 1, b, b, b);
		}

	LPCTSTR str = "test_urban.bmp";

	myimage.Save(str);

}

int pnpoly(int nvert, float* vertx, float* verty, float testx, float testy)
{
	int i, j, c = 0;
	for (i = 0, j = nvert - 1; i < nvert; j = i++) {
		if (((verty[i] > testy) != (verty[j] > testy)) &&
			(testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
			c = !c;
	}
	return c;
}

int pnpoly_cpp(std::vector<point>& points, double test_latitude, double test_longitude)
{
	int i, j, c = 0;
	for (i = 0, j = points.size() - 1; i < points.size(); j = i++) {
		if (((points[i].longitude > test_longitude) != (points[j].longitude > test_longitude)) &&
			(test_latitude < (points[j].latitutde - points[i].latitutde) * (test_longitude - points[i].longitude) / (points[j].longitude - points[i].longitude) + points[i].latitutde))
			c = !c;
	}
	return c;
}

inline bool in_exclusion(std::vector<polygon_winding>& forestExclusionVector, dsf_polygon poly)
{
	for (auto winding : poly.polygon_windings)
	{
		for (auto p : winding.polygon_points)
		{
			for (auto ex : forestExclusionVector)
			{
				if (pnpoly_cpp(ex.polygon_points, p.latitutde, p.longitude) > 0) return true;
			}
		}
	}
	return false;
}

sar_field_big* ReadElevation(std::string file_name, int width, int height, int bpp, float scale, int offset)
{
	sar_field_big* p_buffer = new sar_field_big[width * height];
	unsigned char* p_cbuffer = new unsigned char[width * height * bpp];

	memset(p_buffer, 0, width * height);

	

	FILE* infile = fopen(file_name.c_str(), "rb");

	if (infile == 0)
	{
		return NULL;
	}


	fread(p_cbuffer, bpp, width * height, infile);

	fclose(infile);



	for (int h = 0; h < (height); h++)
		for (int w = 0; w < (width); w++)
		{
			//unsigned char byte;

			p_buffer[(h * width) + w].elevation = 0;

			for (int b_index = 0; b_index < bpp; b_index++)
			{


				//p_buffer[((elev_height - h - 1)*elev_width) + w] += p_cbuffer[(elev_bpp*((h*elev_width) + w)) + b_index]  << (8*b_index);
				p_buffer[(h * width) + w].elevation += p_cbuffer[(bpp * ((h * width) + w)) + b_index] << (8 * b_index);
			}
			//p_buffer[(h*elev_width) + w] = (unsigned int) ((((float)p_buffer[index]) * elev_scale) + elev_offset);
		}

	delete[] p_cbuffer;

	return p_buffer;


}

inline float GetDistance(int x1, int y1, int x2, int y2)
{
	float delta_x = x1 - x2;
	float delta_y = y1 - y2;

	float distance = sqrt((delta_x * delta_x) + (delta_y * delta_y));
	return distance;
}

inline float ElevationInterpolation(int x, int y, int x1, int y1, float v1, int x2, int y2, float v2, int x3, int y3, float v3, int x4, int y4, float v4)
{
	float d1 = GetDistance(x, y, x1, y1);
	float d2 = GetDistance(x, y, x2, y2);
	float d3 = GetDistance(x, y, x3, y3);
	float d4 = GetDistance(x, y, x4, y4);

	// Invert
	d1 = 1 / (d1 + 0.0001);
	d2 = 1 / (d2 + 0.0001);
	d3 = 1 / (d3 + 0.0001);
	d4 = 1 / (d4 + 0.0001);

	// Normalize to sum 1
	float sum = d1 + d2 + d3 + d4;

	d1 = d1 / sum;
	d2 = d2 / sum;
	d3 = d3 / sum;
	d4 = d4 / sum;


	float value = d1 * v1 +
		d2 * v2 +
		d3 * v3 +
		d4 * v4;

	return value;

}

void InterpolateElevation(sar_field_big* p_sar, int elev_width, int elev_height)
{
	for (int x = 0; x < elev_width; x++)
	{
		for (int y = 0; y < elev_width; y++)
		{
			if (p_sar[(y * elev_height) + x].elevation == 0)
			{
				bool abort = false;
				bool found = false;
				int xla = -1, yla = -1;
				int xra = -1, yra = -1;
				int xlb = -1, ylb = -1;
				int xrb = -1, yrb = -1;


				for (int step_size = 1; (step_size <= 100) && (abort == false) && (found == false); step_size++)
				{
					for (int point = 0; (point < (step_size * 2)) && (abort == false) && (found == false); point++)
					{
						int a = step_size;
						if (point > (step_size + 1))
							a = step_size - (point - (step_size + 1));

						int b = point > (step_size + 1) ? step_size + 1 : point;

						if ((xla == -1) && (yla == -1))
						{
							int x1 = x - a;
							int y1 = y - b;

							if ((x1 < 0) || (x1 >= elev_width) || (y1 < 0) || (y1 > elev_height))
							{
								abort = true;
							}
							else
							{
								if (p_sar[(y1 * elev_height) + x1].elevation > 0)
								{
									xla = x1;
									yla = y1;
								}
							}
						}

						if ((xra == -1) && (yra == -1))
						{
							int x1 = x + b;
							int y1 = y - a;

							if ((x1 < 0) || (x1 >= elev_width) || (y1 < 0) || (y1 > elev_height))
							{
								abort = true;
							}
							else
							{
								if (p_sar[(y1 * elev_height) + x1].elevation > 0)
								{
									xra = x1;
									yra = y1;
								}
							}
						}

						if ((xlb == -1) && (ylb == -1))
						{
							int x1 = x - b;
							int y1 = y + a;

							if ((x1 < 0) || (x1 >= elev_width) || (y1 < 0) || (y1 >= elev_height)) //DEBUGX
							{
								abort = true;
							}
							else
							{
								if (p_sar[(y1 * elev_height) + x1].elevation > 0)
								{
									xlb = x1;
									ylb = y1;
								}
							}
						}

						if ((xrb == -1) && (yrb == -1))
						{
							int x1 = x + a;
							int y1 = y - b;

							if ((x1 < 0) || (x1 >= elev_width) || (y1 < 0) || (y1 > elev_height))
							{
								abort = true;
							}
							else
							{
								if (p_sar[(y1 * elev_height) + x1].elevation > 0)
								{
									xrb = x1;
									yrb = y1;
								}
							}
						}

						if ((xla != -1) && (yla != -1) &&
							(xra != -1) && (yra != -1) &&
							(xlb != -1) && (ylb != -1) &&
							(xrb != -1) && (yrb != -1))
						{
							found = true;




						}

					}


				}

				if (found == true)
				{
					int x_start = xla;
					int x_stop = xla;

					x_start = min(x_start, xra);
					x_start = min(x_start, xlb);
					x_start = min(x_start, xrb);

					x_stop = max(x_stop, xra);
					x_stop = max(x_stop, xlb);
					x_stop = max(x_stop, xrb);

					int y_start = yla;
					int y_stop = yla;

					y_start = min(y_start, yra);
					y_start = min(y_start, ylb);
					y_start = min(y_start, yrb);

					y_stop = max(y_stop, yra);
					y_stop = max(y_stop, ylb);
					y_stop = max(y_stop, yrb);

					float x_vektor[4] = { xla, xra, xlb, xrb };
					float y_vektor[4] = { yla, yra, ylb, yrb };

					for (int xv = x_start; xv < x_stop; xv++)
					{
						for (int yv = y_start; yv < y_stop; yv++)
						{
							if (p_sar[(yv * elev_height) + xv].elevation == 0)
							{
								if (pnpoly(4, x_vektor, y_vektor, xv, yv) > 0)
								{
									float elev = ElevationInterpolation(xv, yv, xla, yla, p_sar[(yla * elev_height) + xla].elevation,
										xra, yra, p_sar[(yra * elev_height) + xra].elevation,
										xlb, ylb, p_sar[(ylb * elev_height) + xlb].elevation,
										xrb, yrb, p_sar[(yrb * elev_height) + xrb].elevation);

									p_sar[(yv * elev_height) + xv].elevation = elev;
									p_sar[(yv * elev_height) + xv].is_usable = p_sar[(yla * elev_height) + xla].is_usable && p_sar[(yra * elev_height) + xra].is_usable && p_sar[(ylb * elev_height) + xlb].is_usable && p_sar[(yrb * elev_height) + xrb].is_usable;
								}
							}
						}
					}

				}
			}

		}
	}
}

double calc_distance_m(double lat1, double long1, double lat2, double long2)
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

bool WaypointThread::getFlatPoint(std::vector<dsf_polygon>& forest_vector, sar_field_big* p_sar, int width, int height, double lat_origin, double long_origin, int act_x, int act_y, int delta_x, int delta_y, waypoint& w_out, int index, bool is_slingload, bool is_water)
{
	bool found = false;

	static float delta_lat = 0;
	static float delta_long = 0;
	static float m_dev_max = 0;
	static float m_dev_min = 0;

	double m_MPerLat = HRM_INV;
	double m_MPerLon = HRM_INV;
	double m_LatPerM = HRM_INV;
	double m_LonPerM = HRM_INV;

	if ((m_LatPerM == HRM_INV) || (m_LonPerM == HRM_INV))
	{
		m_MPerLat = abs(calc_distance_m(lat_origin, long_origin, lat_origin + 1.0, long_origin));
		m_MPerLon = abs(calc_distance_m(lat_origin, long_origin, lat_origin, long_origin + 1.0));
	}

	delta_lat = 2.0f / (float)(width - 1);
	delta_long = 2.0f / (float)(height - 1);

	if (is_slingload == false)
	{

		float m_dev_lat = sin((m_WaypointData.m_SARFlatSurfaceSlope * M_PI) / 180.0f) * m_MPerLat * delta_lat;
		float m_dev_long = sin((m_WaypointData.m_SARFlatSurfaceSlope * M_PI) / 180.0f) * m_MPerLon * delta_long;
		m_dev_max = min(m_dev_lat, m_dev_long);
		m_dev_min = 0;
	}
	else
	{
		float m_dev_lat = sin((m_WaypointData.m_SlingMaxSlope * M_PI) / 180.0f) * m_MPerLat * delta_lat;
		float m_dev_long = sin((m_WaypointData.m_SlingMaxSlope * M_PI) / 180.0f) * m_MPerLon * delta_long;
		m_dev_max = min(m_dev_lat, m_dev_long);

		m_dev_lat = sin((m_WaypointData.m_SlingMinSlope * M_PI) / 180.0f) * m_MPerLat * delta_lat;
		m_dev_long = sin((m_WaypointData.m_SlingMinSlope * M_PI) / 180.0f) * m_MPerLon * delta_long;
		m_dev_min = max(m_dev_lat, m_dev_long);

	}


	for (int center_x = act_x + 1; (center_x < (act_x + delta_x)) && (found == false); center_x++)
	{
		for (int center_y = act_y + 1; (center_y < (act_y + delta_y)) && (found == false); center_y++)
		{

			//int center_x = act_x + (delta_x / 2);
			//int center_y = act_y + (delta_y / 2);

			double head_x = p_sar[(center_y * width) + center_x].elevation - p_sar[(center_y * width) + center_x + 1].elevation;
			double head_y = p_sar[(center_y * width) + center_x].elevation - p_sar[((center_y + 1) * width) + center_x].elevation;


			if ((center_x > 0) && (center_x < width) && (center_y > 0) && (center_y < height))
			{

				bool flat_surface = false;

				sar_field_big& sar_c = p_sar[(center_y * width) + center_x];
				sar_field_big* sar_around[8];

				int distance = 1;

				if (is_water)
					distance = 1;



				sar_around[0] = &p_sar[((center_y - distance) * width) + (center_x - distance)];
				sar_around[1] = &p_sar[((center_y - distance) * width) + (center_x)];
				sar_around[2] = &p_sar[((center_y - distance) * width) + (center_x + distance)];

				sar_around[3] = &p_sar[((center_y)*width) + (center_x - distance)];
				sar_around[4] = &p_sar[((center_y)*width) + (center_x + distance)];

				sar_around[5] = &p_sar[((center_y + distance) * width) + (center_x - distance)];
				sar_around[6] = &p_sar[((center_y + distance) * width) + (center_x)];
				sar_around[7] = &p_sar[((center_y + distance) * width) + (center_x + distance)];


				float min_elevation = sar_c.elevation;
				float max_elevation = sar_c.elevation;

				for (int index = 0; index < 8; index++)
				{
					min_elevation = min(min_elevation, sar_around[index]->elevation);
					max_elevation = max(max_elevation, sar_around[index]->elevation);
				}

				float delta_elevation = max_elevation - min_elevation;

				if (is_water == sar_c.is_water)
				{

					if (is_water == true)
					{
						found = true;

						for (int index = 0; index < 8; index++)
						{
							if (sar_around[index]->is_water == false)
								found = false;
						}
					}

					else if ((delta_elevation <= m_dev_max) && (delta_elevation >= m_dev_min) && (sar_c.is_usable == true))
					{
						found = true;

						for (int index = 0; index < 8; index++)
						{
							if (sar_around[index]->is_usable == false)
								found = false;
						}
					}
				}

				//index++;

				if (found == true)
				{

					double latitude = lat_origin + ((((double)center_y)/* + 0.2*/) / ((double)(height - 1)));
					double longitude = long_origin + ((((double)center_x)/* + 0.2*/) / ((double)(width - 1)));

					w_out.name = "P_" + std::to_string(((int)head_x)) + "_" + std::to_string(((int)head_y)) + "_" + std::to_string(((int)delta_elevation)) + "_";
					w_out.latitude = latitude;
					w_out.longitude = longitude;

					w_out.latitude_head = latitude + (head_y * 200.0 / m_MPerLat);
					w_out.longitude_head = longitude + (head_x * 200.0 / m_MPerLon);

					return true;

					/*std::ofstream fms_file;
					fms_file.open("test.fms");
					fms_file.precision(9);

					fms_file << "I" << std::endl;
					fms_file << "1100 Version" << std::endl;
					fms_file << "CYCLE " << 1809 << std::endl;
					fms_file << "DEP " << std::endl;
					fms_file << "DES " << std::endl;
					fms_file << "NUMENR 3" << std::endl;

					fms_file << "28 " << "J" << index << " DEP 25000.000000 " << latitude << " " << longitude << std::endl;
					fms_file.close();*/


				}
			}
		}
	}

	/*if ((++index < 3) && (found == false))
	{
		if (found == false) found = getFlatPoint(forest_vector, p_sar, width, height, lat_origin, long_origin, act_x, act_y, delta_x / 2, delta_y / 2, w_out, index);
		if (found == false) found = getFlatPoint(forest_vector, p_sar, width, height, lat_origin, long_origin, act_x + (delta_x / 2), act_y, delta_x / 2, delta_y / 2, w_out, index);
		if (found == false) found = getFlatPoint(forest_vector, p_sar, width, height, lat_origin, long_origin, act_x, act_y + (delta_y / 2), delta_x / 2, delta_y / 2, w_out, index);
		if (found == false) found = getFlatPoint(forest_vector, p_sar, width, height, lat_origin, long_origin, act_x + (delta_x / 2), act_y + (delta_y / 2), delta_x / 2, delta_y / 2, w_out, index);
	}*/

	return found;

}

WaypointThread::WaypointThread(WaypointCreationData waypointDataIn)

{
	m_WaypointData = waypointDataIn;
	mp_UrbanField = new urban_field[HRM_U_SECTOR * HRM_U_SECTOR];

	int elev_width = dem_width;
	int elev_height = dem_height;
	m_Sections = m_WaypointData.m_Sections;
	
	mp_sar = new sar_field_big[dem_height * dem_width];


	for (int index = 0; index < (elev_height * elev_width); index++)
	{
		mp_sar[index].is_usable = true;
	}
}

WaypointThread::~WaypointThread()
{
	delete[] mp_UrbanField;
	delete[] mp_sar;
}

void WaypointThread::DoEvents(void)
{
	/*MSG msg;
	while (PeekMessage(&msg, m_WaypointData.m_dialog->GetSafeHwnd(), 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
	}*/
}

void WaypointThread::RunComputation(int index)
{

	int index_lat = m_WaypointData.m_Lat;
	int index_lon = m_WaypointData.m_Lon;
	bool del_files = false;

	int scenery_index = 1;

	std::string hrm_path = m_WaypointData.m_XP11_Path + "Resources\\plugins\\HRM\\CustomScenery\\";
	m_progress = 0;

	std::string fms_waypoint_filename = "";

	fms_waypoint_filename += index_lat < 0 ? "-" : "+";
	if ((index_lat < 10) && (index_lat > -10)) fms_waypoint_filename += "0";
	fms_waypoint_filename += std::to_string(abs(index_lat));

	fms_waypoint_filename += index_lon < 0 ? "-" : "+";
	if ((index_lon < 100) && (index_lon > -100)) fms_waypoint_filename += "0";
	if ((index_lon < 10) && (index_lon > -10)) fms_waypoint_filename += "0";
	fms_waypoint_filename += std::to_string(abs(index_lon));

	m_thread_name = fms_waypoint_filename;
	m_thread_prefix = " " + m_thread_name + ": ";


	if ((exists_test(hrm_path + "done_" + fms_waypoint_filename + ".txt") == true) && (m_WaypointData.m_do_not_recompute == true))
	{
		
		m_output_messages.push(getTime() + m_thread_prefix + "Tile already computed");
		m_running = false;
		return;
	}


	for (auto path : m_WaypointData.m_SceneryPathList)
	{
		CheckStop();
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


		// NEW ---------------------------------------------------------------------

		std::string command;

		hrm_path = m_WaypointData.m_XP11_Path + "Resources\\plugins\\HRM\\CustomScenery\\";
		std::string tmp_path = m_WaypointData.m_tmp_path + "\\" + dsf_filename + "\\" + std::to_string(scenery_index) + "\\"; //m_WaypointData.m_XP11_Path + "Resources\\plugins\\HRM\\CustomScenery\\tmp\\";
		scenery_index++;
		std::string dsf_full_path = path + "\\Earth nav data\\" + dsf_filename + "\\" + waypoint_filename;
		std::string dsf_new_path = tmp_path + "\\Earth nav data\\" + dsf_filename + "\\" + waypoint_filename;
		std::string dsf_work_dir = tmp_path + "\\Earth nav data\\" + dsf_filename + "\\";
		std::string dsf_out_path = dsf_work_dir + "out\\" + waypoint_filename + ".dsf";

		if (exists_test(dsf_full_path + ".dsf") == true)
		{
			m_output_messages.push(getTime() + m_thread_prefix + "Found: " + dsf_full_path + ".dsf");

			//command = "rd /s /q \"" + tmp_path + "\"";
			//system(command.c_str());

			command = "mkdir \"" + dsf_work_dir + "\"";
			system(command.c_str());
			command = "copy \"" + dsf_full_path + ".dsf\" \"" + dsf_new_path + ".dsf";
			system(command.c_str());
			
			DoEvents();
			command = "7z e \"" + dsf_new_path + ".dsf\" -o\"" + dsf_work_dir + "out\" -aoa";
			system(command.c_str());

			if (exists_test(dsf_out_path) == true)
			{
				m_output_messages.push(getTime() + m_thread_prefix + "Converting: " + dsf_new_path + ".dsf");
				DoEvents();
				command = "DSFTool --dsf2text \"" + dsf_work_dir + "out\\" + waypoint_filename + ".dsf\" \"" + dsf_work_dir + "out\\" + waypoint_filename + ".txt\"";
				system(command.c_str());

				AnalyzeFile("" + dsf_work_dir + "out\\" + waypoint_filename + ".txt", "" + dsf_work_dir + "out\\");

				if (del_files == true)
				{
					command = "del \"" + dsf_work_dir + "out\\*.txt\"";
					system(command.c_str());
				}

			}
			else
			{
				m_output_messages.push(getTime() + m_thread_prefix + "Converting: " + dsf_new_path + ".dsf");
				DoEvents();
				command = "DSFTool --dsf2text \"" + dsf_new_path + ".dsf\" \"" + dsf_new_path + ".txt\"";
				system(command.c_str());

				AnalyzeFile(dsf_new_path + ".txt", dsf_work_dir);

				if (del_files == true)
				{
					command = "del \"" + dsf_new_path + ".txt\"";
					system(command.c_str());
				}
			}

		}

		



		// NEW ---------------------------------------------------------------------


		/*hrm_path = m_WaypointData.m_XP11_Path + "Resources\\plugins\\HRM\\CustomScenery\\";
		std::string dsf_full_path = path + "\\Earth nav data\\" + dsf_filename + "\\" + waypoint_filename;
		std::string dsf_work_dir = path + "\\Earth nav data\\" + dsf_filename + "\\";
		std::string dsf_out_path = dsf_work_dir + "out\\" + waypoint_filename + ".dsf";

		if (exists_test(dsf_full_path + ".dsf") == true)
		{
			writeOutput("Found: " + dsf_full_path + ".dsf", m_OutputList);
			DoEvents();
			std::string command = "7z e \"" + dsf_full_path + ".dsf\" -o\"" + dsf_work_dir + "out\" -aoa";
			system(command.c_str());

			if (exists_test(dsf_out_path) == true)
			{
				writeOutput("Converting: " + dsf_full_path + ".dsf", m_OutputList);
				DoEvents();
				command = "DSFTool --dsf2text \"" + dsf_work_dir + "out\\" + waypoint_filename + ".dsf\" \"" + dsf_work_dir + "out\\" + waypoint_filename + ".txt\"";
				system(command.c_str());

				AnalyzeFile("" + dsf_work_dir + "out\\" + waypoint_filename + ".txt", "" + dsf_work_dir + "out\\");

				if (del_files == true)
				{
					command = "del \"" + dsf_work_dir + "out\\*.txt\"";
					system(command.c_str());
				}

			}
			else
			{
				writeOutput("Converting: " + dsf_full_path + ".dsf", m_OutputList);
				DoEvents();
				command = "DSFTool --dsf2text \"" + dsf_full_path + ".dsf\" \"" + dsf_full_path + ".txt\"";
				system(command.c_str());

				AnalyzeFile(dsf_full_path + ".txt", dsf_work_dir);

				if (del_files == true)
				{
					command = "del \"" + dsf_full_path + ".txt\"";
					system(command.c_str());
				}
			}
			
		}
		m_ProgressBar.SetPos(0);
		*/

		
		
		

		

		//Nicht vergessen: Extrahiertes DSF File wieder löschen
	}

	if ((m_TerrainDataFound == false) && (m_ElevationFileFound == false))
	{
		m_output_messages.push(getTime() + m_thread_prefix + "No Terrain Data found - Skipping Computation");
		m_running = false;
		return;
	}

	

	if (m_WaypointData.m_StreetMissions == true) AnalyzeStreetWaypoints(hrm_path + "street_" + fms_waypoint_filename + ".fms");
	if (m_WaypointData.m_UrbanMissions == true) AnalyzeUrbanWaypoints(hrm_path + "urban_" + fms_waypoint_filename + ".fms");
	if (m_WaypointData.m_SARMissions == true) AnalyzeSARWaypoints(hrm_path + "sar_" + fms_waypoint_filename + ".fms", false);
	if (m_WaypointData.m_SARMissions == true) AnalyzeSARWaypoints(hrm_path + "water_" + fms_waypoint_filename + ".fms", false, true);
	if (m_WaypointData.m_SlingMissions == true) AnalyzeSARWaypoints(hrm_path + "sling_" + fms_waypoint_filename + ".fms", true);


	std::ofstream fms_file;
	fms_file.open(hrm_path + "done_" + fms_waypoint_filename + ".txt");
	if (fms_file.is_open())
	{
		fms_file << "Done" << std::endl;
		fms_file.close();
	}
	m_output_messages.push(getTime() + m_thread_prefix + "Tile Finished");
	DoEvents();
	m_running = false;
}


/*
	std::ofstream fms_file;
	fms_file.open(fms_filename);
	if (fms_file.is_open())
		{
		fms_file << "Done" << std::endl;
		fms_file.close();
		}

*/

void WaypointThread::AnalyzeStreetWaypoints(std::string fms_filename)
{
	

	if (m_StreetWaypointVector.size() > 0)
	{
		m_output_messages.push(getTime() + m_thread_prefix + "StreetWaypointGeneration: " + fms_filename);
		DoEvents();

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

			int step_size = m_StreetWaypointVector.size() / m_WaypointData.m_WaypointsMax;
			if (step_size <= 0) step_size = 1;

			for (int index = 0; index < m_StreetWaypointVector.size(); index += step_size)
			{
				DoEvents();
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
		m_output_messages.push(getTime() + m_thread_prefix + "No Streets found: " + fms_filename);
	}
}

void WaypointThread::AnalyzeUrbanWaypoints(std::string fms_filename)
{
	if (m_ConsideredStreetJunctions.size() > 0)
	{
		m_output_messages.push(getTime() + m_thread_prefix + "UrbanWaypointGeneration: " + fms_filename);
		DoEvents();

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

			int step_size = m_ConsideredStreetJunctions.size() / m_WaypointData.m_WaypointsMax;
			if (step_size <= 0) step_size = 1;

			for (int index = 0; index < m_ConsideredStreetJunctions.size(); index += step_size)
			{
				street_junction& p_junction = m_ConsideredStreetJunctions[index];
				fms_file << "28 " << "J" << index << " DEP 25000.000000 " << p_junction.latitutdec << " " << p_junction.longitudec << std::endl;
				fms_file << "28 " << "J" << index << "HEAD" << " DEP 25000.000000 " << p_junction.latitutde2 << " " << p_junction.longitude2 << std::endl;

			}

			fms_file.close();
		}
	}
	else
	{
		m_output_messages.push(getTime() + m_thread_prefix + "No Urban Crossings found: " + fms_filename);
	}
}

void WaypointThread::AnalyzeSARWaypoints(std::string fms_filename, bool sling_load, bool find_water)
{
	if (sling_load == false)
	{
		m_output_messages.push(getTime() + m_thread_prefix + "SARWaypointGeneration: " + fms_filename);
	}
	else
	{
		m_output_messages.push(getTime() + m_thread_prefix + "SlingWaypointGeneration: " + fms_filename);
	}
	DoEvents();

	std::vector<waypoint> considered_points;

	double delta_lat = 1.0 / ((double)(m_elev_height - 1));
	double delta_long = 1.0 / ((double)(m_elev_width - 1));


	if (m_ElevationFileFound == true) {
		if (mp_sar != NULL) delete mp_sar;
		mp_sar = ReadElevation(m_elev_filename, m_elev_width, m_elev_height, m_elev_bpp, m_elev_scale, m_elev_offset);
	}
	else InterpolateElevation(mp_sar, m_elev_width, m_elev_height);



	/*for (auto p : m_water_points)
	{
		p.latitutde -= m_WaypointData.m_Lat;
		p.longitude -= m_WaypointData.m_Lon;

		int x = min(p.longitude * (m_elev_width - 1), m_elev_width - 1);
		int y = min(p.latitutde * (m_elev_height - 1), m_elev_height - 1);

		int x_start = max(0, x - HRM_SAR_WATER_DIST);
		int x_stop = min(m_elev_width - 1, x + HRM_SAR_WATER_DIST);

		int y_start = max(0, y - HRM_SAR_WATER_DIST);
		int y_stop = min(m_elev_height - 1, y + HRM_SAR_WATER_DIST);

		for (x = x_start; x <= x_stop; x++)
			for (y = y_start; y <= y_stop; y++)
				mp_sar[(y * m_elev_height) + x].is_water = true;

	}*/

	if (find_water == false)
	{

		for (auto point_vector : m_StreetPoints)
		{
			CheckStop();

			int x_start = abs(point_vector.longitude - m_WaypointData.m_Lon) / delta_long;
			int x_stop = abs(point_vector.longitude - m_WaypointData.m_Lon) / delta_long;

			int y_start = abs(point_vector.latitutde - m_WaypointData.m_Lat) / delta_lat;
			int y_stop = abs(point_vector.latitutde - m_WaypointData.m_Lat) / delta_lat;

			x_start = max(x_start - HRM_SAR_URBAN_DIST, 0);
			y_start = max(y_start - HRM_SAR_URBAN_DIST, 0);

			x_stop = min(x_stop + HRM_SAR_URBAN_DIST, m_elev_width);
			y_stop = min(y_stop + HRM_SAR_URBAN_DIST, m_elev_height);

			for (int x = x_start; x < x_stop; x++)
			{
				for (int y = y_start; y < y_stop; y++)
				{
					mp_sar[(y * m_elev_height) + x].is_urban = true;
				}

			}

		}
		m_output_messages.push(getTime() + m_thread_prefix + "SAR - Streets done: " + fms_filename);





		for (auto poly_vector : m_UrbanVector)
		{
			CheckStop();
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

					int x_start = abs(pmin.longitude - m_WaypointData.m_Lon) / delta_long;
					int x_stop = abs(pmax.longitude - m_WaypointData.m_Lon) / delta_long;

					int y_start = abs(pmin.latitutde - m_WaypointData.m_Lat) / delta_lat;
					int y_stop = abs(pmax.latitutde - m_WaypointData.m_Lat) / delta_lat;

					x_start = max(x_start - HRM_SAR_URBAN_DIST, 0);
					y_start = max(y_start - HRM_SAR_URBAN_DIST, 0);

					x_stop = min(x_stop + HRM_SAR_URBAN_DIST, m_elev_width);
					y_stop = min(y_stop + HRM_SAR_URBAN_DIST, m_elev_height);




					for (int x = x_start; x < x_stop; x++)
					{
						for (int y = y_start; y < y_stop; y++)
						{
							mp_sar[(y * m_elev_height) + x].is_urban = true;
						}

					}
				}
			}

		}
		m_output_messages.push(getTime() + m_thread_prefix + "SAR - Urban Crossings done: " + fms_filename);
	}

	if (sling_load == false)

	{

		for (auto windings : m_ForestVector)
		{
			CheckStop();
			// Get Rectangular Border
			//for (auto current_winding : windings.forest_windings)
			if (windings.polygon_windings.size() > 0)
			{
				polygon_winding& current_winding = windings.polygon_windings[0];
				//paint_poly(current_winding.forest_polygon_points, p_sar, tile_lat, tile_long, elev_width, elev_height);

				point pmax = current_winding.polygon_points[0];
				point pmin = current_winding.polygon_points[0];

				for (auto p : current_winding.polygon_points)
				{
					pmax.latitutde = max(pmax.latitutde, p.latitutde);
					pmax.longitude = max(pmax.longitude, p.longitude);
					pmin.latitutde = min(pmin.latitutde, p.latitutde);
					pmin.longitude = min(pmin.longitude, p.longitude);
				}
				int i = 0;

				if (pmax.longitude > -112) {		
					i++;
				}

				/*int x_start = abs(pmin.longitude - m_WaypointData.m_Lon) / delta_long; //XXX
				int x_stop = abs(pmax.longitude - m_WaypointData.m_Lon) / delta_long;

				int y_start = abs(pmin.latitutde - m_WaypointData.m_Lat) / delta_lat;
				int y_stop = abs(pmax.latitutde - m_WaypointData.m_Lat) / delta_lat;*/

				int x_start = (pmin.longitude - m_WaypointData.m_Lon) / delta_long; //XXX
				int x_stop = (pmax.longitude - m_WaypointData.m_Lon) / delta_long;

				int y_start = (pmin.latitutde - m_WaypointData.m_Lat) / delta_lat;
				int y_stop = (pmax.latitutde - m_WaypointData.m_Lat) / delta_lat;

				x_start = max(x_start - HRM_SAR_URBAN_DIST, 0);
				y_start = max(y_start - HRM_SAR_URBAN_DIST, 0);

				x_start = min(x_start, m_elev_width-1);
				y_start = min(y_start, m_elev_width-1);

				x_stop = min(x_stop + HRM_SAR_URBAN_DIST, m_elev_width-1);
				y_stop = min(y_stop + HRM_SAR_URBAN_DIST, m_elev_height-1);

				x_stop = max(x_stop, 0);
				y_stop = max(y_stop, 0);

				for (int x = x_start; x < x_stop; x++)
				{
					for (int y = y_start; y < y_stop; y++)
					{
						CheckStop();
						double f_lat = m_WaypointData.m_Lat + ((double)y)  / ((double)(m_elev_height - 1));
						double f_long = m_WaypointData.m_Lon + ((double)x)  / ((double)(m_elev_width - 1));

						bool is_forest = false;
						bool in_poly = false;

						// Add last point
						//current_winding.forest_polygon_points.push_back(current_winding.forest_polygon_points[0]);

						// Check if point is in forest
						if (pnpoly_cpp(current_winding.polygon_points, f_lat, f_long) > 0)
						{
							is_forest = true;
							in_poly = true;
						}

						// ToDo: Store IsForest separately and invert on forest entry -> maybe clearings are stored separately in ORBX and US_Forests

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

						//if (is_forest == true)
						if (in_poly == true)
							mp_sar[(y * m_elev_height) + x].is_forest = is_forest; //!mp_sar[(y * m_elev_height) + x].is_forest;

					}

				}


			}




		}
		m_output_messages.push(getTime() + m_thread_prefix + "SAR - Forests done: " + fms_filename);
	}


	// Water Vertices
	for (auto current_winding : m_WaterVector)
	{
		CheckStop();

		//polygon_winding& current_winding = windings.polygon_windings[0];
		//paint_poly(current_winding.forest_polygon_points, p_sar, tile_lat, tile_long, elev_width, elev_height);

		point pmax = current_winding.polygon_points[0];
		point pmin = current_winding.polygon_points[0];

		for (auto p : current_winding.polygon_points)
		{
			pmax.latitutde = max(pmax.latitutde, p.latitutde);
			pmax.longitude = max(pmax.longitude, p.longitude);
			pmin.latitutde = min(pmin.latitutde, p.latitutde);
			pmin.longitude = min(pmin.longitude, p.longitude);
		}

		int x_start = abs(pmin.longitude - m_WaypointData.m_Lon) / delta_long;
		int x_stop = abs(pmax.longitude - m_WaypointData.m_Lon) / delta_long;

		int y_start = abs(pmin.latitutde - m_WaypointData.m_Lat) / delta_lat;
		int y_stop = abs(pmax.latitutde - m_WaypointData.m_Lat) / delta_lat;

		x_start = max(x_start - HRM_SAR_URBAN_DIST, 0);
		y_start = max(y_start - HRM_SAR_URBAN_DIST, 0);

		x_stop = min(x_stop + HRM_SAR_URBAN_DIST, m_elev_width);
		y_stop = min(y_stop + HRM_SAR_URBAN_DIST, m_elev_height);

		for (int x = x_start; x < x_stop; x++)
		{
			for (int y = y_start; y < y_stop; y++)
			{
				double f_lat = m_WaypointData.m_Lat + ((double)y) / ((double)(m_elev_height - 1));
				double f_long = m_WaypointData.m_Lon + ((double)x) / ((double)(m_elev_width - 1));

				// Check if point is in water vertex
				if (pnpoly_cpp(current_winding.polygon_points, f_lat, f_long) > 0)
				{
					mp_sar[(y * m_elev_height) + x].is_water = true;
				}
			}

		}

	}

	m_output_messages.push(getTime() + m_thread_prefix + "SAR - Water done: " + fms_filename);


	
	/*
	for (int x = 0; x < m_elev_width; x++)
	{
		for (int y = 0; y < m_elev_height; y++)
		{
			double f_lat = m_WaypointData.m_Lat + ((double)y) / ((double)(m_elev_height - 1));
			double f_long = m_WaypointData.m_Lon + ((double)x)  / ((double)(m_elev_width - 1));

			for (auto poly_vector : m_WaterVector)
			{
				for (auto current_winding : poly_vector.polygon_windings)
					if (pnpoly_cpp(current_winding.polygon_points, f_lat, f_long) > 0)
						mp_sar[(y * m_elev_height) + x].is_water = true;
			}
		}
	}*/
	
	m_output_messages.push(getTime() + m_thread_prefix + "SAR - Marking usable tiles: " + fms_filename);

	for (int x = 0; x < m_elev_width; x++)
	{
		for (int y = 0; y < m_elev_height; y++)
		{
			if ((mp_sar[(y * m_elev_height) + x].is_forest == true) ||
				//(mp_sar[(y * m_elev_height) + x].is_water == true) ||
				(mp_sar[(y * m_elev_height) + x].is_urban == true)// ||
				//(mp_sar[(y * m_elev_height) + x].elevation < m_WaypointData.m_SARMinAlt)
				)
			{
				mp_sar[(y * m_elev_height) + x].is_usable = false;
			}
		}

	}

	m_output_messages.push(getTime() + m_thread_prefix + "SAR - Start Search: " + fms_filename);


	write_bmp_usable(mp_sar, m_elev_width, m_elev_height);
	write_bmp_forest(mp_sar, m_elev_width, m_elev_height);
	write_bmp_water(mp_sar, m_elev_width, m_elev_height);
	write_bmp_urban(mp_sar, m_elev_width, m_elev_height);

	int delta_h = (m_elev_height - 1) / m_WaypointData.m_Sections;
	int delta_w = (m_elev_width - 1) / m_WaypointData.m_Sections;

	for (int x = 1; x < (HRM_SAR_SECTOR_BIG - 1); x+=2)
	{
		for (int y = 1; y < (HRM_SAR_SECTOR_BIG - 1); y+=2)
		{
			CheckStop(); // This block takes very long
			waypoint w_act;
			if (getFlatPoint(m_ForestVector, mp_sar, m_elev_width, m_elev_height, m_WaypointData.m_Lat, m_WaypointData.m_Lon, x * delta_w, y * delta_h, delta_w, delta_h, w_act, 0, sling_load, find_water))
			{

				considered_points.push_back(w_act);
			}


		}
	}

	m_output_messages.push(getTime() + m_thread_prefix + "SAR - Search Finished: " + fms_filename);


	if (considered_points.size() > 0)
	{
		m_output_messages.push(getTime() + m_thread_prefix + "Writing SAR File");
		DoEvents();
		CheckStop();

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

			int step_size = considered_points.size() / m_WaypointData.m_WaypointsMax;
			if (step_size <= 0) step_size = 1;

			for (int index = 0; index < considered_points.size(); index += step_size)
			{
				waypoint& p = considered_points[index];
				fms_file << "28 " << p.name << index << " DEP 30000.000000 " << p.latitude << " " << p.longitude << std::endl;
				fms_file << "28 " << p.name << "HEAD_" << index << " DEP 30000.000000 " << p.latitude_head << " " << p.longitude_head << std::endl;
			}

			fms_file.close();
		}

		if (validation_file == true) {
			std::ifstream f_pre_v("pre_validate.txt");
			std::ifstream f_post_v("post_validate.txt");
			std::stringstream ss_pre, ss_post;
			ss_pre << f_pre_v.rdbuf();
			ss_post << f_post_v.rdbuf();

			std::string pre_string = ss_pre.str();
			std::string post_string = ss_post.str();

			std::string objects = "";
			std::string children = "";

			int step_size = considered_points.size() / m_WaypointData.m_WaypointsMax;
			if (step_size <= 0) step_size = 1;

			int counter = 5;

			for (int index = 0; index < considered_points.size(); index += step_size)
			{
				objects += "    <object class=\"WED_ObjPlacement\" id=\"" + std::to_string(counter) + "\" parent_id=\"4\">\n";
				objects += "      <children/>\n";
				objects += "      <hierarchy name=\"comm_tower_25m_3.obj\" locked=\"0\" hidden=\"0\"/>\n";
				
				waypoint& p = considered_points[index];
				objects += "      <point latitude=\"" + std::to_string(p.latitude) + "\" longitude=\"" + std::to_string(p.longitude) + "\" heading=\"0.0\"/>\n";
				objects += "      <obj_placement custom_msl=\"0\" msl=\"0.0\" resource=\"simheaven/landmarks/chimney_250m.obj\" show_level=\"1 Default\"/>\n";
				objects += "    </object>\n";

				children += "        <child id=\"" + std::to_string(counter) + "\"/>\n";

				counter++;
			}

			std::ofstream val_file;
			val_file.open(fms_filename + ".xml");
			if (val_file.is_open())
			{
				val_file << pre_string << std::endl;
				val_file << children;
				val_file << "      </children>\n";
				val_file << "      <hierarchy name=\"world\" locked=\"0\" hidden=\"0\"/>\n";
				val_file << "    </object>\n";
				val_file << objects;
				val_file << post_string << std::endl;

				val_file.close();
			}


		}
	}
	else
	{
		m_output_messages.push(getTime() + m_thread_prefix + "No Points found, skipping file");
		DoEvents();
	}
}



void WaypointThread::CheckStreetWaypoint(double lat1, double long1, double lat2, double long2, int sub_type)
{
	CheckStop();
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

void WaypointThread::CheckStop()
{
	if (m_stop > 0) std::terminate();
}

void WaypointThread::AnalyzeFile(std::string filename, std::string workpath)
{
	if (m_TerrainDataFound == true)
	{
		m_output_messages.push(getTime() + m_thread_prefix + "After Terrain -> Skipped: " + filename);
		DoEvents();
		return;
	}

	m_progress = 0;
	DoEvents();

	m_output_messages.push(getTime() + m_thread_prefix + "Reading from Disk: " + filename);
	DoEvents();

	std::ifstream test_file(filename);
	int count = std::count(std::istreambuf_iterator<char>(test_file), std::istreambuf_iterator<char>(), '\n');
	if (count == 0) count = 1;
	test_file.close();

	std::ifstream dsf_file(filename);
	std::string line_string;
	
	m_output_messages.push(getTime() + m_thread_prefix + "Analyzing: " + filename);
	DoEvents();

	
	int progress_pos = 0;
	m_progress = progress_pos;
	DoEvents();

	for (int index = 0; index < MAX_POLYGON_DEF; index++)
	{
		m_PolygonDefinitions[index] = PolygonDef::Invalid;
	}

	//std::list<point> vector_points;





	double long_1 = HRM_INV;
	double lat_1 = HRM_INV;
	double long_2 = HRM_INV;
	double lat_2 = HRM_INV;

	bool is_water = false;

	//int last_water_x = 0;
	//int last_water_y = 0;
	int water_vertex_point = 0;

	polygon_winding water_polygon;


	//std::vector<waypoint> considered_points;

	std::vector<int> water_defs;

	water_defs.push_back(0); // Zero is always Water

	int elevation_pos = 0;
	bool elevation_pos_found = false;
	bool is_overlay = false;
	bool is_valid_terrain = false;
	int poly_index = 0; // Changed from 1
	int raster_count = 0;
	int elevation_file_count = 0;
	int terrain_def_count = 0;

	polygon_winding currentForestExclusion;
	std::vector<polygon_winding> currentForestExclusionVector;
	bool forestExclusionFound = false;

	polygon_winding currentStreetExclusion;
	bool streetExclusionFound = false;


	while (std::getline(dsf_file, line_string))
	{
		CheckStop();
		int current_pos = (int) ( 1000.0 * (((double) progress_pos) / ((double) count)));
		m_progress = current_pos;
		DoEvents();

		progress_pos++;


		std::stringstream line_stream(line_string);

		std::string item_1 = "";
		std::string item_2 = "";

		line_stream >> item_1;

		if (item_1.compare("PROPERTY") == 0)
		{
			line_stream >> item_2;

			if ((item_2.find("overlay") != std::string::npos) || (item_2.find("sim/overlay") != std::string::npos))
			{
				is_overlay = true;
			}

			if ((item_2.find("exclude_for") != std::string::npos) || (item_2.find("sim/exclude_for") != std::string::npos))
			{
				
				std::string exclusion_value;
				point p1, p2, p3, p4;

				double lon_1, lon_2, lat_1, lat_2;

				line_stream >> exclusion_value;

				std::string value = exclusion_value.substr(0, exclusion_value.find("/"));
				lon_1 = std::stod(value);
				exclusion_value.erase(0, exclusion_value.find("/") + 1);

				value = exclusion_value.substr(0, exclusion_value.find("/"));
				lat_1 = std::stod(value);
				exclusion_value.erase(0, exclusion_value.find("/") + 1);

				value = exclusion_value.substr(0, exclusion_value.find("/"));
				lon_2 = std::stod(value);
				exclusion_value.erase(0, exclusion_value.find("/") + 1);

				lat_2 = std::stod(exclusion_value);

				/*p1.longitude = lon_1;
				p1.latitutde = lat_1;

				p2.longitude = lon_2;
				p2.latitutde = lat_1;

				p3.longitude = lon_2;
				p3.latitutde = lat_2;

				p4.longitude = lon_1;
				p4.latitutde = lat_2;*/

				polygon_winding forest_winding;

				p1.longitude = min(lon_1, lon_2);
				p1.latitutde = max(lat_1, lat_2);

				p2.longitude = max(lon_1, lon_2);
				p2.latitutde = max(lat_1, lat_2);

				p3.longitude = max(lon_1, lon_2);
				p3.latitutde = min(lat_1, lat_2);

				p4.longitude = min(lon_1, lon_2);
				p4.latitutde = min(lat_1, lat_2);

				forest_winding.polygon_points.push_back(p1);
				forest_winding.polygon_points.push_back(p2);
				forest_winding.polygon_points.push_back(p3);
				forest_winding.polygon_points.push_back(p4);


				/*currentForestExclusion.polygon_points.push_back(p1);
				currentForestExclusion.polygon_points.push_back(p2);
				currentForestExclusion.polygon_points.push_back(p3);
				currentForestExclusion.polygon_points.push_back(p4);*/

				//forestExclusionFound = true;

				currentForestExclusionVector.push_back(forest_winding);

			}

			if ((item_2.find("exclude_net") != std::string::npos) || (item_2.find("sim/exclude_net") != std::string::npos))
			{

				std::string exclusion_value;
				point p1, p2, p3, p4;

				double lon_1, lon_2, lat_1, lat_2;

				line_stream >> exclusion_value;

				std::string value = exclusion_value.substr(0, exclusion_value.find("/"));
				lon_1 = std::stod(value);
				exclusion_value.erase(0, exclusion_value.find("/") + 1);

				value = exclusion_value.substr(0, exclusion_value.find("/"));
				lat_1 = std::stod(value);
				exclusion_value.erase(0, exclusion_value.find("/") + 1);

				value = exclusion_value.substr(0, exclusion_value.find("/"));
				lon_2 = std::stod(value);
				exclusion_value.erase(0, exclusion_value.find("/") + 1);

				lat_2 = std::stod(exclusion_value);

				/*p1.longitude = lon_1;
				p1.latitutde = lat_1;

				p2.longitude = lon_2;
				p2.latitutde = lat_1;

				p3.longitude = lon_2;
				p3.latitutde = lat_2;

				p4.longitude = lon_1;
				p4.latitutde = lat_2;


				currentStreetExclusion.polygon_points.push_back(p1);
				currentStreetExclusion.polygon_points.push_back(p2);
				currentStreetExclusion.polygon_points.push_back(p3);
				currentStreetExclusion.polygon_points.push_back(p4);

				forestExclusionFound = true;*/

				polygon_winding street_winding;

				p1.longitude = min(lon_1, lon_2);
				p1.latitutde = max(lat_1, lat_2);

				p2.longitude = max(lon_1, lon_2);
				p2.latitutde = max(lat_1, lat_2);

				p3.longitude = max(lon_1, lon_2);
				p3.latitutde = min(lat_1, lat_2);

				p4.longitude = min(lon_1, lon_2);
				p4.latitutde = min(lat_1, lat_2);

				street_winding.polygon_points.push_back(p1);
				street_winding.polygon_points.push_back(p2);
				street_winding.polygon_points.push_back(p3);
				street_winding.polygon_points.push_back(p4);

				m_StreetExclusionVector.push_back(street_winding);

			}

		}
		else if (item_1.compare("TERRAIN_DEF") == 0)
		{
			/*if ((is_overlay == false) && (m_TerrainDataFound == false))
			{
				m_TerrainDataFound = true;
				is_valid_terrain = true;
			}*/

			//if (is_valid_terrain == true)
			{

				line_stream >> item_2;
				if ((item_2.find("Water") != std::string::npos) || (item_2.find("_water_") != std::string::npos) || (item_2.find("_sea_") != std::string::npos))
					water_defs.push_back(terrain_def_count);

				terrain_def_count++;
			}
		}

		else if (item_1.compare("RASTER_DEF") == 0)
		{
			line_stream >> item_2;

			if (item_2.compare("elevation") == 0)
			{
				elevation_pos_found = true;
				elevation_pos = raster_count;
				m_TerrainDataFound = true;
			}
			raster_count++;

		}
		else if (item_1.compare("RASTER_DATA") == 0)
		{
			if (elevation_pos_found == true)
			{
				if (elevation_file_count == elevation_pos)
				{
					m_ElevationFileFound = true;

					line_stream >> item_2; // version
					line_stream >> item_2; // bpp
					sscanf_s(item_2.c_str(), "bpp=%i", &m_elev_bpp);

					line_stream >> item_2; // flags

					line_stream >> item_2; // width
					sscanf_s(item_2.c_str(), "width=%i", &m_elev_width);

					line_stream >> item_2; // height
					sscanf_s(item_2.c_str(), "height=%i", &m_elev_height);

					line_stream >> item_2; // scale
					sscanf_s(item_2.c_str(), "scale=%f", &m_elev_scale);

					line_stream >> item_2; // offset
					sscanf_s(item_2.c_str(), "offset=%f", &m_elev_offset);
					
					getline(line_stream, m_elev_filename);
					m_elev_filename.erase(0, 1);
					
					//line_stream >> m_elev_filename;

					//m_elev_filename = workpath + m_elev_filename;

				}
				elevation_file_count++;
			}
		}


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Urban, Forests, etc.
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		else if (item_1.compare("POLYGON_DEF") == 0)
		{
			if ((is_overlay == false) && (m_TerrainDataFound == false))
			{
				m_TerrainDataFound = true;
				is_valid_terrain = true;
			}

			//if ((is_valid_terrain == true) || (is_overlay == true))
			{
				line_stream >> item_2;
				if (poly_index < MAX_POLYGON_DEF)
				{
					if (item_2.find(".obj") != std::string::npos)			  m_PolygonDefinitions[poly_index] = PolygonDef::Object;
					if (item_2.find(".fac") != std::string::npos)			  m_PolygonDefinitions[poly_index] = PolygonDef::Facade;
					if ((item_2.find(".for") != std::string::npos) 
						//&& (item_2.find("Grass") == std::string::npos)
						//&& (item_2.find("grass") == std::string::npos)
						//&& (item_2.find("lib/vegetation/trees/deciduous/shrubs") == std::string::npos)
						//&& (item_2.find("shrubs") == std::string::npos)
						//&& (item_2.find("Shrubs") == std::string::npos)
						//&& (item_2.find("low") == std::string::npos)
						//&& (item_2.find("small") == std::string::npos)
						)	// Mr. TTK likes paving Montana with grass and shrubs   	  
																			  m_PolygonDefinitions[poly_index] = PolygonDef::Forest;
					if (item_2.find(".bch") != std::string::npos)	    	  
						m_PolygonDefinitions[poly_index] = PolygonDef::Beach;
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

			//if ((is_valid_terrain == true) || (is_overlay == true))
			{
				bool end_segment = false;
				is_water = false;

				int poly_type = -1;
				int poly_density = 300;
				line_stream >> poly_type;
				line_stream >> poly_density;



				dsf_polygon current_polygon;
				polygon_winding current_winding;

				while ((end_segment == false))
				{
					CheckStop();
					std::getline(dsf_file, line_string);
					m_progress = current_pos;
					DoEvents();

					progress_pos++;

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


						// Urban Waypoints

						if ((lat_1 != HRM_INV) && (long_1 != HRM_INV))
						{
							if ((m_MPerLat == HRM_INV) || (m_MPerLon == HRM_INV))
							{
								m_MPerLat = abs(calc_distance_m(lat_1, long_1, lat_1 + 1.0, long_1));
								m_MPerLon = abs(calc_distance_m(lat_1, long_1, lat_1, long_1 + 1.0));
							}


							int segment = GetUrbanSegment(lat_1, long_1);

							point p;
							p.latitutde = lat_1;
							p.longitude = long_1;

							mp_UrbanField[segment].urban_polygon_points.push_back(p);
						}
					}

					else if (item_1.compare("END_POLYGON") == 0)
					{
						//***************************************************************************************************
						//ToDo: Add Exclusion Zone Check
						//***************************************************************************************************


						if ((poly_type >= 0) && (current_polygon.polygon_windings.size() > 0))
						{
							if (m_PolygonDefinitions[poly_type] == PolygonDef::Forest)
							{
								if (in_exclusion(m_ForestExclusionVector, current_polygon) == false)
								{
									if (poly_density <= 255)
										m_ForestVector.push_back(current_polygon);
								}						
							}
							else if (m_PolygonDefinitions[poly_type] == PolygonDef::Beach)			
								m_BeachVector.push_back(current_polygon);
							else if (m_PolygonDefinitions[poly_type] == PolygonDef::Facade)			m_UrbanVector.push_back(current_polygon);
							else if (m_PolygonDefinitions[poly_type] == PolygonDef::AutogenBlock)	m_UrbanVector.push_back(current_polygon);
							else if (m_PolygonDefinitions[poly_type] == PolygonDef::AutogenString)	m_UrbanVector.push_back(current_polygon);
							//else if (m_PolygonDefinitions[poly_type] == PolygonDef::Water)			m_WaterVector.push_back(current_polygon);
						}

						end_segment = true;
					}
				}
			}

		}
		
		else if (item_1.compare("BEGIN_PATCH") == 0)
		{
			int terrain_type = 0;
			double near_lod = 0;
			double far_lod = 0;
			int flags = 0;

			water_vertex_point = 0;
			water_polygon.polygon_points.clear();

			line_stream >> terrain_type;
			line_stream >> near_lod;
			line_stream >> far_lod;
			line_stream >> flags;

			is_water = false;

			// Water! Check Vector
			if ((terrain_type == 0) || (flags == 2)) //flag 2 means overlay, but no collision
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

				for (auto t_number : water_defs)
				{
					if (t_number == terrain_type)
						is_water = true;
				}
			}
		}

		else if (item_1.compare("END_PATCH") == 0)
		{
			//is_water = false;
			water_vertex_point = 0;
			water_polygon.polygon_points.clear();
		}
		else if (item_1.compare("BEGIN_PRIMITIVE") == 0)
		{
			//is_water = false;
			water_vertex_point = 0;
			water_polygon.polygon_points.clear();
		}

		else if (item_1.compare("PATCH_VERTEX") == 0)
		{

			if ((is_overlay == false) && (m_TerrainDataFound == false))
			{
				m_TerrainDataFound = true;
				is_valid_terrain = true;
			}

			if ((is_valid_terrain == true) && (m_TerrainDataFound == true))
			{
				

				// Calculate if terrain is usable
				double latitude;
				double longitude;
				float elevation;

				line_stream >> longitude;
				line_stream >> latitude;
				line_stream >> elevation;

				int x = min((int)((longitude - m_WaypointData.m_Lon) * ((double)(m_elev_width - 1))), m_elev_width - 1);
				int y = min((int)((latitude - m_WaypointData.m_Lat) * ((double)(m_elev_height - 1))), m_elev_height - 1);

				//if ((y == 868) && (x == 1102))
				//	is_water = true;


				if (is_water)
				{
					point p;
					p.latitutde = latitude;
					p.longitude = longitude;

					water_polygon.polygon_points.push_back(p);
					water_vertex_point++;

					if (water_vertex_point >= 3)
					{
						m_WaterVector.push_back(water_polygon);
						water_polygon.polygon_points.clear();
						water_vertex_point = 0;
					}
					


					//mp_sar[(y * m_elev_height) + x].is_water = true;


					/*
					if ((last_water_x > 0) && (last_water_y > 0))
					{
							

						double step_x = (((double)last_water_x) - ((double)x)) / 100.0;
						double step_y = (((double)last_water_y) - ((double)y)) / 100.0;

						for (double step = 0; step < 100; step++)
						{
							int dx = (step_x * step) + x;
							int dy = (step_y * step) + y;

							mp_sar[(dy * m_elev_height) + dx].is_water = true;
						}
							
							
					}


					// We are working on filling triangles
					vertex_point++;

					if (vertex_point < 3)
					{
						last_water_x = x;
						last_water_y = y;
					}
					else
					{
						vertex_point = 0;
						last_water_x = 0;
						last_water_y = 0;
					}*/


						
						
				}

				if (m_ElevationFileFound == false)
				{
					mp_sar[(y * m_elev_height) + x].elevation = elevation;
				}
				

			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Streets
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		else if (item_1.compare("BEGIN_SEGMENT") == 0)
		{

			int type = -1;
			int sub_type = -1;
			int node_id = -1;
			int elevation_1 = 0;
			int elevation_2 = 0;
			
			std::vector<street_section> current_street_sections;


			line_stream >> type;
			line_stream >> sub_type;
			line_stream >> node_id;
			line_stream >> long_1;
			line_stream >> lat_1;
			line_stream >> elevation_1;

			point p;
			p.longitude = long_1;
			p.latitutde = lat_1;
			m_StreetPoints.push_back(p);


			bool end_segment = false;

			

			if (sub_type > 0)
			{
				while ((end_segment == false))
				{
					CheckStop();
					std::getline(dsf_file, line_string);
					std::stringstream line_stream_2(line_string);

					std::string item_2 = "";
					line_stream_2 >> item_2;

					if (item_2.compare("SHAPE_POINT") == 0)
					{
						line_stream_2 >> long_2;
						line_stream_2 >> lat_2;
						line_stream_2 >> elevation_2;

						point prev = m_StreetPoints[m_StreetPoints.size() - 1];

						p.longitude = long_2;
						p.latitutde = lat_2;

						double delta_lat = p.latitutde - prev.latitutde;
						double delta_long = p.longitude - prev.longitude;

						for (double index = 0; index < 10.0; index++)
						{
							point p_delta;

							p_delta.latitutde = prev.latitutde + (delta_lat * index / 10.0);
							p_delta.longitude = prev.longitude + (delta_long * index / 10.0);

							m_StreetPoints.push_back(p_delta);
						}



						m_StreetPoints.push_back(p);


						// No Bridges!!!
						if ((elevation_1 == 0) && (elevation_2 == 0))
						{
							// Street
							CheckStreetWaypoint(lat_1, long_1, lat_2, long_2, sub_type);

							// Urban
							if ((sub_type > 10) && (sub_type < 80))
							{
								if ((abs((lat_1 - lat_2) * m_MPerLat) > HRM_U_STREET_DIST) || (abs((long_1 - long_2) * m_MPerLon) > HRM_U_STREET_DIST))
								{
									street_section sec;
									sec.latitutde1 = lat_1;
									sec.longitude1 = long_1;
									sec.latitutde2 = lat_2;
									sec.longitude2 = long_2;
									current_street_sections.push_back(sec);

									sec.latitutde2 = lat_1;
									sec.longitude2 = long_1;
									sec.latitutde1 = lat_2;
									sec.longitude1 = long_2;
									current_street_sections.push_back(sec);
								}
							}



						}

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

						// Every Street point is handled like a water point to avoid stuff nearby

						point prev = m_StreetPoints[m_StreetPoints.size() - 1];

						p.longitude = long_2;
						p.latitutde = lat_2;

						double delta_lat = p.latitutde - prev.latitutde;
						double delta_long = p.longitude - prev.longitude;

						for (double index = 0; index < 10.0; index++)
						{
							point p_delta;

							p_delta.latitutde = prev.latitutde + (delta_lat * index / 10.0);
							p_delta.longitude = prev.longitude + (delta_long * index / 10.0);

							m_StreetPoints.push_back(p_delta);
						}

						m_StreetPoints.push_back(p);

						// No Bridges!!!
						if ((elevation_1 == 0) && (elevation_2 == 0))
						{
							bool is_excluded = false;

							for (auto ex : m_StreetExclusionVector)
							{
								if (pnpoly_cpp(ex.polygon_points, lat_1, long_1) > 0) is_excluded = true;
								if (pnpoly_cpp(ex.polygon_points, lat_2, long_2) > 0) is_excluded = true;
							}

							
							if (is_excluded == false) {

								// Street
								CheckStreetWaypoint(lat_1, long_1, lat_2, long_2, sub_type);

								// Urban
								if ((sub_type > 10) && (sub_type < 80))
								{
									if ((abs((lat_1 - lat_2) * m_MPerLat) > HRM_U_STREET_DIST) || (abs((long_1 - long_2) * m_MPerLon) > HRM_U_STREET_DIST))
									{

										street_section sec;
										sec.latitutde1 = lat_1;
										sec.longitude1 = long_1;
										sec.latitutde2 = lat_2;
										sec.longitude2 = long_2;
										current_street_sections.push_back(sec);

										sec.latitutde2 = lat_1;
										sec.longitude2 = long_1;
										sec.latitutde1 = lat_2;
										sec.longitude1 = long_2;
										current_street_sections.push_back(sec);
									}

									for (auto const cs1 : current_street_sections)
									{
										int segment = GetUrbanSegment(cs1.latitutde1, cs1.longitude1);

										for (auto const cs2 : mp_UrbanField[segment].urban_street_sections)
										{
											double delta_lat_m = (cs1.latitutde1 - cs2.latitutde1) * m_MPerLat;
											double delta_long_m = (cs1.longitude1 - cs2.longitude1) * m_MPerLon;

											if ((abs(delta_lat_m) < HRM_U_JUNCTION_DIST) && (abs(delta_long_m) < HRM_U_JUNCTION_DIST))
											{
												double angle1 = atan2(cs1.latitutde1 - cs1.latitutde2, cs1.longitude1 - cs1.longitude2) * 180 / M_PI;
												double angle2 = atan2(cs2.latitutde1 - cs2.latitutde2, cs2.longitude1 - cs2.longitude2) * 180 / M_PI;

												if (angle1 < 0) angle1 += 360;
												if (angle2 < 0) angle2 += 360;

												double angle = angle1 - angle2;
												if (angle < 0) angle += 360;

												if (abs(angle - 90.0) <= HRM_U_ANGLE_VAR)
												{
													street_junction sj;

													sj.latitutdec = cs1.latitutde1;
													sj.longitudec = cs1.longitude1;

													sj.latitutde2 = cs1.latitutde2;
													sj.longitude2 = cs1.longitude2;

													sj.latitutde3 = cs2.latitutde2;
													sj.longitude3 = cs2.longitude2;

													mp_UrbanField[segment].urban_street_junctions.push_back(sj);
												}
												else if (abs(angle - 270.0) <= HRM_U_ANGLE_VAR)
												{
													street_junction sj;
													sj.latitutdec = cs1.latitutde1;
													sj.longitudec = cs1.longitude1;

													sj.latitutde3 = cs1.latitutde2;
													sj.longitude3 = cs1.longitude2;

													sj.latitutde2 = cs2.latitutde2;
													sj.longitude2 = cs2.longitude2;

													mp_UrbanField[segment].urban_street_junctions.push_back(sj);
												}
											}

										}
									}

									for (auto cs1 : current_street_sections)
									{
										int segment = GetUrbanSegment(cs1.latitutde1, cs1.longitude1);

										mp_UrbanField[segment].urban_street_sections.push_back(cs1);
									}
								}
							}

						}

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

	// Get Junctions that are urban
	

	for (int segment = 0; segment < (HRM_U_SECTOR * HRM_U_SECTOR); segment++)
	{
		for (auto const junction : mp_UrbanField[segment].urban_street_junctions)
		{
			bool is_urban = false;
			for (auto poly : mp_UrbanField[segment].urban_polygon_points)
			{
				double delta_lat_m = (junction.latitutdec - poly.latitutde) * m_MPerLat;
				double delta_long_m = (junction.longitudec - poly.longitude) * m_MPerLon;

				if ((abs(delta_lat_m) < HRM_U_URBAN_DIST) && (abs(delta_long_m) < HRM_U_URBAN_DIST))
				{
					is_urban = true;
					break;
				}
			}

			if (is_urban == true)
			{
				m_ConsideredStreetJunctions.push_back(junction);
			}
		}
	}

	for (auto winding : currentForestExclusionVector) 
	{
		m_ForestExclusionVector.push_back(winding);
	}

	/*if (forestExclusionFound == true)
	{
		m_ForestExclusionVector.push_back(currentForestExclusion);
	}

	if (streetExclusionFound == true)
	{
		m_StreetExclusionVector.push_back(currentStreetExclusion);
	}*/
	m_output_messages.push(getTime() + m_thread_prefix + "Finished Analyzing: " + filename);
	DoEvents();

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



	for (auto p : m_water_points)
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

	m_progress = 0;
	DoEvents();
}
