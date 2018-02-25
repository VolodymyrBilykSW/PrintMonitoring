
// PrintMonitoring.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"	


// CPrintMonitoringApp:
class CPrintMonitoringApp : public CWinApp
{
public:
	CPrintMonitoringApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnExitApp();
	afx_msg void OnOpenlog();
};

extern CPrintMonitoringApp theApp;
