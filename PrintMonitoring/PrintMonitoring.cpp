
// PrintMonitoring.cpp : Определяет поведение классов для приложения.
//

#include "stdafx.h"
#include "PrintMonitoring.h"
#include "PrintMonitoringDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPrintMonitoringApp

BEGIN_MESSAGE_MAP(CPrintMonitoringApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_COMMAND(ID_EXIT_APP, &CPrintMonitoringApp::OnExitApp)
	ON_COMMAND(ID_OPENLOG, &CPrintMonitoringApp::OnOpenlog)
END_MESSAGE_MAP()


// создание CPrintMonitoringApp

CPrintMonitoringApp::CPrintMonitoringApp()
{
	// поддержка диспетчера перезагрузки
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}


// Единственный объект CPrintMonitoringApp

CPrintMonitoringApp theApp;


// инициализация CPrintMonitoringApp

BOOL CPrintMonitoringApp::InitInstance()
{
	// InitCommonControlsEx() требуется для Windows XP, если манифест
	// приложения использует ComCtl32.dll версии 6 или более поздней версии для включения
	// стилей отображения.  В противном случае будет возникать сбой при создании любого окна.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Выберите этот параметр для включения всех общих классов управления, которые необходимо использовать
	// в вашем приложении.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Создать диспетчер оболочки, в случае, если диалоговое окно содержит
	// представление дерева оболочки или какие-либо его элементы управления.
	CShellManager *pShellManager = new CShellManager;

	// Активация визуального диспетчера "Классический Windows" для включения элементов управления MFC
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Стандартная инициализация
	SetRegistryKey(_T("Bilyk Studio"));

	CPrintMonitoringDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: Введите код для обработки закрытия диалогового окна
		//  с помощью кнопки "ОК"
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Введите код для обработки закрытия диалогового окна
		//  с помощью кнопки "Отмена"
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Предупреждение. Не удалось создать диалоговое окно, поэтому работа приложения неожиданно завершена.\n");
		TRACE(traceAppMsg, 0, "Предупреждение. При использовании элементов управления MFC для диалогового окна невозможно #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Удалить диспетчер оболочки, созданный выше.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Поскольку диалоговое окно закрыто, возвратите значение FALSE, чтобы можно было выйти из
	//  приложения вместо запуска генератора сообщений приложения.
	return FALSE;
}



void CPrintMonitoringApp::OnExitApp()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE, 0, 0);
}


void CPrintMonitoringApp::OnOpenlog()
{
	if (GetFileAttributes(L"log.txt") != DWORD(-1)) {
		ShellExecute(NULL, NULL, L"log.txt", NULL, NULL, SW_RESTORE);
	}
	else
		MessageBox(NULL, L"File hasn`t created yet", L"Error", MB_ICONERROR);
}
