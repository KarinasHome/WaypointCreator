
// WaypointCreatorDlg.cpp: Implementierungsdatei
//

#include "pch.h"
#include "framework.h"
#include "WaypointCreator.h"
#include "WaypointCreatorDlg.h"
#include "afxdialogex.h"


#include <string>
#include <sstream>

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
	PathToXP11EditBrowse.SetWindowText("G:\\XPlane 11 Test");

	m_LatStart.SetWindowText(CA2CT(std::to_string(m_LatStartValue).c_str()));
	m_LatStop.SetWindowText(CA2CT(std::to_string(m_LatStopValue).c_str()));
	m_LonStart.SetWindowText(CA2CT(std::to_string(m_LonStartValue).c_str()));
	m_LonStop.SetWindowText(CA2CT(std::to_string(m_LonStopValue).c_str()));

	FlatSlopeMax.SetWindowText(CA2CT(std::to_string(m_FlatSlopeMaxValue).c_str()));
	SlingSlopeMin.SetWindowText(CA2CT(std::to_string(m_SlingSlopeMinValue).c_str()));
	SlingSlopeMax.SetWindowText(CA2CT(std::to_string(m_SlingSlopeMaxValue).c_str()));

	m_Sections.SetWindowText(CA2CT(std::to_string(m_SectionsValue).c_str()));

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
	CheckLimits(m_Sections, m_SectionsValue, 5, 400);
}

void CWaypointCreatorDlg::OnBnClickedBtnCreateWaypoints()
{
	int latStart = m_LatStartValue;
	int latStop = m_LatStopValue;

	int lonStart = m_LonStartValue;
	int lonStop = m_LonStopValue;

	m_SceneryPathList.clear();

	m_OutputList.ResetContent();

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

	/*for (auto path : m_SceneryPathList)
	{
		m_OutputList.AddString(CA2CT(path.c_str()));
	}*/

	WaypointCreationData wpData;

	wpData.m_StreetMissions = m_StreetEnable.GetCheck() > 0 ? true : false;
	wpData.m_UrbanMissions = m_UrbanEnable.GetCheck() > 0 ? true : false;
	wpData.m_SARMissions = m_SarEnable.GetCheck() > 0 ? true : false;
	wpData.m_SlingMissions = m_SlingEnable.GetCheck() > 0 ? true : false;

	wpData.m_Sections = m_SectionsValue;
	wpData.m_SARFlatSurfaceSlope = (float) m_FlatSlopeMaxValue;
	wpData.m_SlingMinSlope = (float) m_SlingSlopeMinValue;
	wpData.m_SlingMaxSlope = (float) m_SlingSlopeMaxValue;

	wpData.m_SceneryPathList = m_SceneryPathList;


	wpData.m_XP11_Path = m_XP11_Path;
	wpData.m_dialog = this;

	for (int lat = latStart; lat <= latStop; lat++)
	{
		for (int lon = lonStart; lon <= lonStop; lon++)
		{
			wpData.m_Lat = lat;
			wpData.m_Lon = lon;

			WaypointThread nextComputation(wpData, m_OutputList, m_ProgressFile);
			nextComputation.RunComputation();

		}
	}
}