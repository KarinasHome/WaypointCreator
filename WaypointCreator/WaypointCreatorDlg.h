
// WaypointCreatorDlg.h: Headerdatei
//

#pragma once


#include <list>
#include <string>
#include "WaypointThread.h"
#include "common.h"
#include <queue>
#include <thread>
#include <mutex>

//#include <boost/property_tree/ini_parser.hpp>


// CWaypointCreatorDlg-Dialogfeld
class CWaypointCreatorDlg : public CDialogEx
{
// Konstruktion
public:
	CWaypointCreatorDlg(CWnd* pParent = nullptr);	// Standardkonstruktor

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WAYPOINTCREATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	std::string m_XP11_Path = "";
	std::list<std::string> m_SceneryPathList;
	WaypointCreationData m_WPData;

	int m_LatStartValue = -90;
	int m_LatStopValue = 90;
	unsigned int m_MaxCores = 1;

	int m_LonStartValue = 0;
	int m_LonStopValue = 0;

	int m_FlatSlopeMaxValue = 5;
	int m_SlingSlopeMinValue = 25;
	int m_SlingSlopeMaxValue = 65;
	int m_SectionsValue = 100;

	int m_Min_Alt = -100;
	int m_Max_Wayp = 100;

	std::vector<std::thread*> mp_std_threads;
	std::vector<WaypointThread*> mp_wp_threads;

	
	

	bool m_EnableNotifications = true;
	void CheckLimits(CEdit& editBox, int& storeValue, int min, int max);
	void SaveIni(void);
	void ReadIni(void);


	CMFCEditBrowseCtrl PathToXP11EditBrowse;
	afx_msg void ReadSceneryInIClicked();
	void ReadSceneryIniFile(std::string scenery_ini_file);
	CListBox m_SceneryList;
	afx_msg void OnBnClickedButtonSelectAll();
	afx_msg void OnBnClickedButtonSelectNone();
	CEdit m_LatStart;
	CEdit m_LatStop;
	CEdit m_LonStart;
	CEdit m_LonStop;
	CButton m_StreetEnable;
	CButton m_UrbanEnable;
	CButton m_SarEnable;
	CButton m_SlingEnable;
	afx_msg void OnBnClickedBtnCreateWaypoints();
	afx_msg void OnEnChangeEditLatStart();
	afx_msg void OnEnChangeEditLatStop();
	afx_msg void OnEnChangeEditLonStart3();
	afx_msg void OnEnChangeEditLonStop();
	CEdit FlatSlopeMax;
	CEdit SlingSlopeMin;
	CEdit SlingSlopeMax;
	afx_msg void OnEnChangeEditFlatSlope();
	afx_msg void OnEnChangeEditSlingMinSlope();
	afx_msg void OnEnChangeEditSlingMaxSlope();
	afx_msg void OnEnKillfocusEditSlingMinSlope();
	afx_msg void OnEnKillfocusEditSlingMaxSlope();
	afx_msg void OnEnKillfocusEditLatStart();
	afx_msg void OnEnKillfocusEditLatStop();
	afx_msg void OnEnKillfocusEditLonStart3();
	afx_msg void OnEnKillfocusEditLonStop();
	afx_msg void OnEnKillfocusEditFlatSlope();
	afx_msg void OnEnChangeEditSections();
	CEdit m_Sections;
	afx_msg void OnEnKillfocusEditSections();
	CListBox m_OutputList;
	CProgressCtrl m_ProgressFile;
	afx_msg void OnEnKillfocusEditMaxWayp();
	afx_msg void OnEnKillfocusEditMinAlt();
	CEdit m_Max_Waypoints;
	CEdit m_SAR_Min_Alt;
	CMFCEditBrowseCtrl PathToTmpEditBrowse;
	CButton m_DoNotRecompute;
	afx_msg void OnBnCloseClickedOk();
	afx_msg void OnClose();
};
