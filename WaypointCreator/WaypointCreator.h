
// WaypointCreator.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'pch.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CWaypointCreatorApp:
// Siehe WaypointCreator.cpp für die Implementierung dieser Klasse
//

class CWaypointCreatorApp : public CWinApp
{
public:
	CWaypointCreatorApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CWaypointCreatorApp theApp;
