#include "pch.h"
#include "WaypointThread.h"

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

				AnalyzeFile("\"" + dsf_work_dir + "out\\" + waypoint_filename + ".txt\"");

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
}

void WaypointThread::AnalyzeFile(std::string filename)
{
	writeOutput("Analyzing: " + filename, m_OutputList);
}
