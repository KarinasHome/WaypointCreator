
// WaypointCreatorDlg.cpp: Implementierungsdatei
//

#include "pch.h"
#include "framework.h"
#include "WaypointCreator.h"
#include "WaypointCreatorDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <fstream>
#include "atlstr.h"
//#include <boost/property_tree/ptree.hpp>



#include <string>
#include <sstream>
#include <array>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::recursive_mutex outputListSharedMutex;


// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWaypointCreatorDlg-Dialogfeld



CWaypointCreatorDlg::CWaypointCreatorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WAYPOINTCREATOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWaypointCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_XP11_PATH, PathToXP11EditBrowse);
	DDX_Control(pDX, IDC_LIST_SCENERY, m_SceneryList);
	DDX_Control(pDX, IDC_EDIT_LAT_START, m_LatStart);
	DDX_Control(pDX, IDC_EDIT_LAT_STOP, m_LatStop);
	DDX_Control(pDX, IDC_EDIT_LON_START3, m_LonStart);
	DDX_Control(pDX, IDC_EDIT_LON_STOP, m_LonStop);
	DDX_Control(pDX, IDC_STREET, m_StreetEnable);
	DDX_Control(pDX, IDC_URBAN, m_UrbanEnable);
	DDX_Control(pDX, IDC_SAR, m_SarEnable);
	DDX_Control(pDX, IDC_SLING, m_SlingEnable);
	DDX_Control(pDX, IDC_EDIT_FLAT_SLOPE, FlatSlopeMax);
	DDX_Control(pDX, IDC_EDIT_SLING_MIN_SLOPE, SlingSlopeMin);
	DDX_Control(pDX, IDC_EDIT_SLING_MAX_SLOPE, SlingSlopeMax);
	DDX_Control(pDX, IDC_EDIT_SECTIONS, m_Sections);
	DDX_Control(pDX, IDC_LIST_OUTPUT, m_OutputList);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressFile);
	DDX_Control(pDX, IDC_EDIT_MAX_WAYP, m_Max_Waypoints);
	DDX_Control(pDX, IDC_EDIT_MIN_ALT, m_SAR_Min_Alt);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_TMP_PATH, PathToTmpEditBrowse);
	DDX_Control(pDX, IDC_RECOMP, m_DoNotRecompute);
}

BEGIN_MESSAGE_MAP(CWaypointCreatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(R, &CWaypointCreatorDlg::ReadSceneryInIClicked)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, &CWaypointCreatorDlg::OnBnClickedButtonSelectAll)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_NONE, &CWaypointCreatorDlg::OnBnClickedButtonSelectNone)
	ON_BN_CLICKED(IDC_BTN_CREATE_WAYPOINTS, &CWaypointCreatorDlg::OnBnClickedBtnCreateWaypoints)
	ON_EN_CHANGE(IDC_EDIT_LAT_START, &CWaypointCreatorDlg::OnEnChangeEditLatStart)
	ON_EN_CHANGE(IDC_EDIT_LAT_STOP, &CWaypointCreatorDlg::OnEnChangeEditLatStop)
	ON_EN_CHANGE(IDC_EDIT_LON_START3, &CWaypointCreatorDlg::OnEnChangeEditLonStart3)
	ON_EN_CHANGE(IDC_EDIT_LON_STOP, &CWaypointCreatorDlg::OnEnChangeEditLonStop)
	ON_EN_CHANGE(IDC_EDIT_FLAT_SLOPE, &CWaypointCreatorDlg::OnEnChangeEditFlatSlope)
	ON_EN_CHANGE(IDC_EDIT_SLING_MIN_SLOPE, &CWaypointCreatorDlg::OnEnChangeEditSlingMinSlope)
	ON_EN_CHANGE(IDC_EDIT_SLING_MAX_SLOPE, &CWaypointCreatorDlg::OnEnChangeEditSlingMaxSlope)
	ON_EN_KILLFOCUS(IDC_EDIT_SLING_MIN_SLOPE, &CWaypointCreatorDlg::OnEnKillfocusEditSlingMinSlope)
	ON_EN_KILLFOCUS(IDC_EDIT_SLING_MAX_SLOPE, &CWaypointCreatorDlg::OnEnKillfocusEditSlingMaxSlope)
	ON_EN_KILLFOCUS(IDC_EDIT_LAT_START, &CWaypointCreatorDlg::OnEnKillfocusEditLatStart)
	ON_EN_KILLFOCUS(IDC_EDIT_LAT_STOP, &CWaypointCreatorDlg::OnEnKillfocusEditLatStop)
	ON_EN_KILLFOCUS(IDC_EDIT_LON_START3, &CWaypointCreatorDlg::OnEnKillfocusEditLonStart3)
	ON_EN_KILLFOCUS(IDC_EDIT_LON_STOP, &CWaypointCreatorDlg::OnEnKillfocusEditLonStop)
	ON_EN_KILLFOCUS(IDC_EDIT_FLAT_SLOPE, &CWaypointCreatorDlg::OnEnKillfocusEditFlatSlope)
	ON_EN_CHANGE(IDC_EDIT_SECTIONS, &CWaypointCreatorDlg::OnEnChangeEditSections)
	ON_EN_KILLFOCUS(IDC_EDIT_SECTIONS, &CWaypointCreatorDlg::OnEnKillfocusEditSections)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_WAYP, &CWaypointCreatorDlg::OnEnKillfocusEditMaxWayp)
	ON_EN_KILLFOCUS(IDC_EDIT_MIN_ALT, &CWaypointCreatorDlg::OnEnKillfocusEditMinAlt)
	ON_BN_CLICKED(IDOK, &CWaypointCreatorDlg::OnBnCloseClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CWaypointCreatorDlg-Meldungshandler

BOOL CWaypointCreatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen.  Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// TODO: Hier zusätzliche Initialisierung einfügen

	

	PathToXP11EditBrowse.EnableFolderBrowseButton();
	PathToXP11EditBrowse.SetWindowText("C:\\X-Plane 12");

	PathToTmpEditBrowse.EnableFolderBrowseButton();
	PathToTmpEditBrowse.SetWindowText("C:\\hrm_tmp\\");

	m_StreetEnable.SetCheck(1);
	m_UrbanEnable.SetCheck(1);
	m_SarEnable.SetCheck(1);
	m_SlingEnable.SetCheck(1);

	ReadIni();

	m_LatStart.SetWindowText(CA2CT(std::to_string(m_LatStartValue).c_str()));
	m_LatStop.SetWindowText(CA2CT(std::to_string(m_LatStopValue).c_str()));
	m_LonStart.SetWindowText(CA2CT(std::to_string(m_LonStartValue).c_str()));
	m_LonStop.SetWindowText(CA2CT(std::to_string(m_LonStopValue).c_str()));

	FlatSlopeMax.SetWindowText(CA2CT(std::to_string(m_FlatSlopeMaxValue).c_str()));
	SlingSlopeMin.SetWindowText(CA2CT(std::to_string(m_SlingSlopeMinValue).c_str()));
	SlingSlopeMax.SetWindowText(CA2CT(std::to_string(m_SlingSlopeMaxValue).c_str()));

	m_Sections.SetWindowText(CA2CT(std::to_string(m_SectionsValue).c_str()));
	m_Max_Waypoints.SetWindowText(CA2CT(std::to_string(m_Max_Wayp).c_str()));
	m_SAR_Min_Alt.SetWindowText(CA2CT(std::to_string(m_Min_Alt).c_str()));

	m_MaxCores = std::thread::hardware_concurrency();
	if (m_MaxCores > 1) m_MaxCores--;
	writeOutput("Number of cores selected for computation: " + std::to_string(m_MaxCores), m_OutputList);

	//m_LatStart.SetMargins()

	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

void CWaypointCreatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
//  den nachstehenden Code, um das Symbol zu zeichnen.  Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CWaypointCreatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CWaypointCreatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWaypointCreatorDlg::CheckLimits(CEdit &editBox, int &storeValue, int min, int max)
{
	if (m_EnableNotifications)
	{
		CPoint curPos = editBox.GetCaretPos();
		CString windowText;
		editBox.GetWindowText(windowText);
		CT2CA pszConvertedAnsiString(windowText);
		std::string text(pszConvertedAnsiString);

		//char buffer[2000];
		int value = atoi(text.c_str());

		if ((str_is_number(text.c_str()) == true) && (value >= min) && (value <= max))
		{
			if (strcmp(text.c_str(), "-") != 0)
				storeValue = value;
		}
		//else if ((strcmp(text.c_str(), "-0") != 0) && (strcmp(text.c_str(), "-") != 0) && (strcmp(text.c_str(), "") != 0))
		{
			m_EnableNotifications = false;
			editBox.SetWindowText(CA2CT(std::to_string(storeValue).c_str()));
			m_EnableNotifications = true;
		}
	}
}

void CWaypointCreatorDlg::ReadSceneryInIClicked()
{
	// TODO: Fügen Sie hier Ihren Handlercode für Benachrichtigungen des Steuerelements ein.
	CString windowText;
	PathToXP11EditBrowse.GetWindowText(windowText);

	CT2CA pszConvertedAnsiString(windowText);

	std::string folder(pszConvertedAnsiString);
	std::string scenery_ini_path = folder + "\\Custom Scenery\\scenery_packs.ini";

	if (windowText.Compare("") == 0)
	{
		MessageBox("No folder selected");
	}
	else if (!exists_test(scenery_ini_path))
	{
		MessageBox("Error: scenery_packs.ini not found");
	}
	else
	{
		m_XP11_Path = folder + "\\";

		ReadSceneryIniFile(scenery_ini_path);
	}

	
}

void CWaypointCreatorDlg::ReadSceneryIniFile(std::string scenery_ini_file)
{
	//MessageBox(L"Start reading ini file");

	m_SceneryList.ResetContent();

	std::ifstream dsf_file(scenery_ini_file);
	std::string line_string;

	m_SceneryPathList.clear();

	while (std::getline(dsf_file, line_string))
	{
		std::stringstream line_stream(line_string);
		std::string item_1 = "";
		line_stream >> item_1;

		if (item_1.compare("SCENERY_PACK") == 0)
		{
			std::string path = "";
			std::getline(line_stream, path);

			path = ltrim(path);

			m_SceneryPathList.push_back(path.c_str());
			m_SceneryList.AddString(CA2CT(path.c_str()));
		}


	}

	m_SceneryList.SelItemRange(1, 0, m_SceneryList.GetCount());

}


void CWaypointCreatorDlg::OnBnClickedButtonSelectAll()
{
	m_SceneryList.SelItemRange(1, 0, m_SceneryList.GetCount());
}


void CWaypointCreatorDlg::OnBnClickedButtonSelectNone()
{
	m_SceneryList.SelItemRange(0, 0, m_SceneryList.GetCount());
}





void CWaypointCreatorDlg::OnEnChangeEditLatStart()
{
	//CheckLimits(m_LatStart, m_LatStartValue, WaypointCreator::LatMin, WaypointCreator::LatMax);
}


void CWaypointCreatorDlg::OnEnChangeEditLatStop()
{
	//CheckLimits(m_LatStop, m_LatStopValue, WaypointCreator::LatMin, WaypointCreator::LatMax);
}


void CWaypointCreatorDlg::OnEnChangeEditLonStart3()
{
	//CheckLimits(m_LonStart, m_LonStartValue, WaypointCreator::LonMin, WaypointCreator::LonMax);
}


void CWaypointCreatorDlg::OnEnChangeEditLonStop()
{
	//CheckLimits(m_LonStop, m_LonStopValue, WaypointCreator::LonMin, WaypointCreator::LonMax);
}


void CWaypointCreatorDlg::OnEnChangeEditFlatSlope()
{
	//CheckLimits(FlatSlopeMax, m_FlatSlopeMaxValue, 0, 90);
}


void CWaypointCreatorDlg::OnEnChangeEditSlingMinSlope()
{
	//CheckLimits(SlingSlopeMin, m_SlingSlopeMinValue, 0, 90);
}


void CWaypointCreatorDlg::OnEnChangeEditSlingMaxSlope()
{
	//CheckLimits(SlingSlopeMax, m_SlingSlopeMaxValue, 0, 90);
}


void CWaypointCreatorDlg::OnEnKillfocusEditSlingMinSlope()
{
	CheckLimits(SlingSlopeMin, m_SlingSlopeMinValue, 0, m_SlingSlopeMaxValue);
}


void CWaypointCreatorDlg::OnEnKillfocusEditSlingMaxSlope()
{
	CheckLimits(SlingSlopeMax, m_SlingSlopeMaxValue, m_SlingSlopeMinValue, 90);
}


void CWaypointCreatorDlg::OnEnKillfocusEditLatStart()
{
	CheckLimits(m_LatStart, m_LatStartValue, WaypointCreator::LatMin, m_LatStopValue);
}


void CWaypointCreatorDlg::OnEnKillfocusEditLatStop()
{
	CheckLimits(m_LatStop, m_LatStopValue, m_LatStartValue, WaypointCreator::LatMax);
}


void CWaypointCreatorDlg::OnEnKillfocusEditLonStart3()
{
	CheckLimits(m_LonStart, m_LonStartValue, WaypointCreator::LonMin, m_LonStopValue);
}


void CWaypointCreatorDlg::OnEnKillfocusEditLonStop()
{
	CheckLimits(m_LonStop, m_LonStopValue, m_LonStartValue, WaypointCreator::LonMax);
}


void CWaypointCreatorDlg::OnEnKillfocusEditFlatSlope()
{
	CheckLimits(FlatSlopeMax, m_FlatSlopeMaxValue, 0, 90);
}


void CWaypointCreatorDlg::OnEnChangeEditSections()
{
	
}


void CWaypointCreatorDlg::OnEnKillfocusEditSections()
{
	CheckLimits(m_Sections, m_SectionsValue, 5, 1000);
}

void CWaypointCreatorDlg::OnBnClickedBtnCreateWaypoints()
{
	int latStart = m_LatStartValue;
	int latStop = m_LatStopValue;

	int lonStart = m_LonStartValue;
	int lonStop = m_LonStopValue;

	m_SceneryPathList.clear();

	m_OutputList.ResetContent();

	CString windowText;
	PathToTmpEditBrowse.GetWindowText(windowText);

	CT2CA pszConvertedAnsiString(windowText);

	std::string folder(pszConvertedAnsiString);

	std::string tmp_path = folder;

	writeOutput("Temp folder: " + tmp_path, m_OutputList);

	std::string command;
	command = "rd /s /q \"" + tmp_path + "\"";
	system(command.c_str());

	int count = m_SceneryList.GetCount();
	for (int index = 0; index < count; index++)
	{
		if (m_SceneryList.GetSel(index) > 0)
		{
			CString text;
			m_SceneryList.GetText(index, text);

			CT2CA pszConvertedAnsiString(text);
			std::string directory_name(pszConvertedAnsiString);

			if (directory_name.rfind("Custom Scenery", 0) == 0)
			{
				directory_name = m_XP11_Path + directory_name;
			}
			std::replace(directory_name.begin(), directory_name.end(), '/', '\\');
			m_SceneryPathList.push_back(directory_name);
		}
	}
	std::string global_scenery = m_XP11_Path + "Global Scenery\\X-Plane 11 Global Scenery\\";
	m_SceneryPathList.push_back(global_scenery);
	global_scenery = m_XP11_Path + "Global Scenery\\X-Plane 12 Global Scenery\\";
	m_SceneryPathList.push_back(global_scenery);

	/*for (auto path : m_SceneryPathList)
	{
		m_OutputList.AddString(CA2CT(path.c_str()));
	}*/

	int max_threads = 4;

	


	for (int lat = latStart; lat <= latStop; lat++)
	{
		for (int lon = lonStart; lon <= lonStop; lon++)
		{
			while (mp_std_threads.size() >= max_threads)
			{
				for (int index = 0; index < mp_std_threads.size(); index++) {
					
					while (mp_wp_threads[index]->m_output_messages.size() > 0) writeOutput(mp_wp_threads[index]->m_output_messages.pop(), m_OutputList);
					
					if (mp_wp_threads[index]->m_running == false)
					{
						writeOutput("Thread finished: " + std::to_string(index), m_OutputList);
						mp_std_threads[index]->join();
						delete mp_wp_threads[index];
						delete mp_std_threads[index];
						mp_std_threads.erase(mp_std_threads.begin() + index);
						mp_wp_threads.erase(mp_wp_threads.begin() + index);

					}
				}

				Sleep(100);
				MSG msg;
				while (PeekMessage(&msg, this->GetSafeHwnd(), 0, 0, PM_REMOVE))
				{
					DispatchMessage(&msg);
				}
			}




			WaypointCreationData wpData;

			wpData.m_StreetMissions = m_StreetEnable.GetCheck() > 0 ? true : false;
			wpData.m_UrbanMissions = m_UrbanEnable.GetCheck() > 0 ? true : false;
			wpData.m_SARMissions = m_SarEnable.GetCheck() > 0 ? true : false;
			wpData.m_SlingMissions = m_SlingEnable.GetCheck() > 0 ? true : false;
			wpData.m_do_not_recompute = m_DoNotRecompute.GetCheck() > 0 ? true : false;

			wpData.m_Sections = m_SectionsValue;
			wpData.m_SARFlatSurfaceSlope = (float)m_FlatSlopeMaxValue;
			wpData.m_SlingMinSlope = (float)m_SlingSlopeMinValue;
			wpData.m_SlingMaxSlope = (float)m_SlingSlopeMaxValue;

			wpData.m_SARMinAlt = m_Min_Alt;
			wpData.m_WaypointsMax = m_Max_Wayp;

			wpData.m_SceneryPathList = m_SceneryPathList;
			wpData.m_tmp_path = tmp_path;

			wpData.m_XP11_Path = m_XP11_Path;
			wpData.m_dialog = this;


			wpData.m_Lat = lat;
			wpData.m_Lon = lon;

			WaypointThread *nextComputation = new WaypointThread(wpData);

			std::thread* p_wp_thread = NULL;

			p_wp_thread = new std::thread(&WaypointThread::RunComputation, nextComputation, 1);
			writeOutput("Next Thread Started", m_OutputList);

			mp_std_threads.push_back(p_wp_thread);
			mp_wp_threads.push_back(nextComputation);

		}
	}

	writeOutput("All Threads Started, waiting for result", m_OutputList);

	while (mp_std_threads.size() > 0)
	{
		for (int index = 0; index < mp_std_threads.size(); index++) {

			while (mp_wp_threads[index]->m_output_messages.size() > 0) writeOutput(mp_wp_threads[index]->m_output_messages.pop(), m_OutputList);

			if (mp_wp_threads[index]->m_running == false)
			{
				writeOutput("Thread finished: " + std::to_string(index), m_OutputList);
				mp_std_threads[index]->join();
				delete mp_wp_threads[index];
				delete mp_std_threads[index];
				mp_std_threads.erase(mp_std_threads.begin() + index);
				mp_wp_threads.erase(mp_wp_threads.begin() + index);

			}
		}

		Sleep(100);
		MSG msg;
		while (PeekMessage(&msg, this->GetSafeHwnd(), 0, 0, PM_REMOVE))
		{
			DispatchMessage(&msg);
		}
	}


	SaveIni();
}

void CWaypointCreatorDlg::OnEnKillfocusEditMaxWayp()
{
	// TODO: Fügen Sie hier Ihren Handlercode für Benachrichtigungen des Steuerelements ein.
	CheckLimits(m_Max_Waypoints, m_Max_Wayp, 10, 10000);
}


void CWaypointCreatorDlg::OnEnKillfocusEditMinAlt()
{
	// TODO: Fügen Sie hier Ihren Handlercode für Benachrichtigungen des Steuerelements ein.
	CheckLimits(m_SAR_Min_Alt, m_Min_Alt, -1000, 10000);
}

void CWaypointCreatorDlg::SaveIni(void) {
	std::ofstream ini_file;
	ini_file.open("wp_creator.ini");
	CString windowText;
	PathToXP11EditBrowse.GetWindowText(windowText);
	CT2CA pszConvertedAnsiString(windowText);
	std::string xp_path(pszConvertedAnsiString);
	PathToTmpEditBrowse.GetWindowText(windowText);
	CT2CA pszConvertedAnsiString2(windowText);
	std::string tmp_path(pszConvertedAnsiString2);

	ini_file << xp_path << "\n";
	ini_file << tmp_path << "\n";
	ini_file << m_LatStartValue << "\t" << m_LatStopValue << "\t" << m_LonStartValue << "\t" << m_LonStopValue << "\t" << m_FlatSlopeMaxValue << "\t" << m_SlingSlopeMinValue << "\t" << m_SlingSlopeMaxValue << "\t" << m_SectionsValue << "\t" << m_Min_Alt << "\t" << m_Max_Wayp << "\n";
	ini_file << (m_StreetEnable.GetCheck() > 0 ? true : false) << "\t";
	ini_file << (m_UrbanEnable.GetCheck() > 0 ? true : false) << "\t";
	ini_file << (m_SarEnable.GetCheck() > 0 ? true : false) << "\t";
	ini_file << (m_SlingEnable.GetCheck() > 0 ? true : false) << "\t";
	ini_file << (m_DoNotRecompute.GetCheck() > 0 ? true : false) << "\n";
	
	
	ini_file.close();

}
void CWaypointCreatorDlg::ReadIni(void) {
	std::ifstream ini_file;
	ini_file.open("wp_creator.ini");
	if (ini_file.is_open()) {
		std::string line;
		std::string tmp_path;
		
		getline(ini_file, line);
		m_XP11_Path = line;
		getline(ini_file, line);
		tmp_path = line;
		getline(ini_file, line);
		std::istringstream iss(line);
		iss >> m_LatStartValue;
		iss >> m_LatStopValue;
		iss >> m_LonStartValue;
		iss >> m_LonStopValue;
		iss >> m_FlatSlopeMaxValue;
		iss >> m_SlingSlopeMinValue;
		iss >> m_SlingSlopeMaxValue;
		iss >> m_SectionsValue;
		iss >> m_Min_Alt;
		iss >> m_Max_Wayp;
		
		getline(ini_file, line);
		std::istringstream iss2(line);

		int res = 0;
		iss2 >> res;
		m_StreetEnable.SetCheck(res);
		iss2 >> res;
		m_UrbanEnable.SetCheck(res);
		iss2 >> res;
		m_SarEnable.SetCheck(res);
		iss2 >> res;
		m_SlingEnable.SetCheck(res);
		iss2 >> res;
		m_DoNotRecompute.SetCheck(res);

		ini_file.close();

		PathToXP11EditBrowse.SetWindowText(m_XP11_Path.c_str());
		PathToTmpEditBrowse.SetWindowText(tmp_path.c_str());
	}

	
}


void CWaypointCreatorDlg::OnBnCloseClickedOk()
{
	writeOutput("Killing Threads", m_OutputList);
	for (int index = 0; index < mp_std_threads.size(); index++) {
		mp_wp_threads[index]->m_stop = true;
		mp_std_threads[index]->join();
		writeOutput("Thread Killed", m_OutputList);
	}
	SaveIni();
	// TODO: Fügen Sie hier Ihren Handlercode für Benachrichtigungen des Steuerelements ein.
	CDialogEx::OnOK();
}


void CWaypointCreatorDlg::OnClose()
{
	// TODO: Fügen Sie hier Ihren Meldungshandlercode ein, und/oder benutzen Sie den Standard.
	writeOutput("Killing Threads", m_OutputList);
	for (int index = 0; index < mp_std_threads.size(); index++) {
		mp_wp_threads[index]->m_stop = true;
		mp_std_threads[index]->join();
		writeOutput("Thread Killed", m_OutputList);
	}
	SaveIni();
	CDialogEx::OnClose();
}
