

// PrintMonitoringDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "PrintMonitoring.h"
#include "PrintMonitoringDlg.h"
#include "afxdialogex.h"
#include "Logic.h"
#include <winreg.h>

#define WM_MYICONNOTIFY (WM_USER+1)
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// диалоговое окно CPrintMonitoringDlg

CPrintMonitoringDlg::CPrintMonitoringDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PRINTMONITORING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_PRINTERICON);
}

void CPrintMonitoringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STOP, btnStop);
	DDX_Control(pDX, IDC_START, btnStart);
	DDX_Control(pDX, IDC_CB_PRINTERS, cbPrinters);
	DDX_Control(pDX, IDC_LB_PRINTLOG, logList);
}

BEGIN_MESSAGE_MAP(CPrintMonitoringDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_CB_PRINTERS, &CPrintMonitoringDlg::OnCbnSelchangeCbPrinters)
	ON_BN_CLICKED(IDC_START, &CPrintMonitoringDlg::BtnStartClick)
	ON_BN_CLICKED(IDC_STOP, &CPrintMonitoringDlg::BtnStopClick)
	ON_BN_CLICKED(IDC_HIDE, &CPrintMonitoringDlg::OnBnClickedHide)
	ON_MESSAGE(WM_MYICONNOTIFY, &CPrintMonitoringDlg::OnIcon)
	ON_BN_CLICKED(IDC_OPENLOG, &CPrintMonitoringDlg::OnBnClickedOpenlog)
	ON_BN_CLICKED(IDC_DELETELOG, &CPrintMonitoringDlg::OnBnClickedDeletelog)
	ON_BN_CLICKED(IDC_CB_AUTORUN, &CPrintMonitoringDlg::OnBnClickedCbAutorun)

END_MESSAGE_MAP()


// обработчики сообщений CPrintMonitoringDlg

BOOL CPrintMonitoringDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, TRUE);			// Мелкий значок

	// Create icon in tray
	OnBnClickedHide();

	HKEY isKey;

	if (RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &isKey) == ERROR_SUCCESS) {
		if (RegQueryValueEx(isKey, L"PrintMonitor", NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			// check on checkBox
			((CButton *)GetDlgItem(IDC_CB_AUTORUN))->SetCheck(BST_CHECKED);
			RegCloseKey(isKey);
		}
	}

	//-------------------------------------------------------------------
	// Loading list of available printers

	DWORD pCount;
	auto printers = LoadPrinters(pCount);

	for (DWORD i = 0; i < pCount; i++)
	{
		cbPrinters.AddString((printers+i)->pPrinterName);
	}

	// Creating titles for listBox
	logList.InsertColumn(0, _T("Data"), LVCFMT_CENTER, 75);
	logList.InsertColumn(1, _T("Time"), LVCFMT_CENTER, 70);
	logList.InsertColumn(2, _T("Printer"), LVCFMT_CENTER, 160);
	logList.InsertColumn(3, _T("Document"), LVCFMT_CENTER, 190);
	logList.InsertColumn(4, _T("Computer/User"), LVCFMT_CENTER, 190);
	logList.InsertColumn(5, _T("Number"), LVCFMT_CENTER, 50);
	//-------------------------------------------------------------------

	return TRUE;  // Return TRUE if focus is not passed to the control
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CPrintMonitoringDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CPrintMonitoringDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// Printer picker field
void CPrintMonitoringDlg::OnCbnSelchangeCbPrinters()
{
	btnStart.EnableWindow(TRUE);
}

// Button 'start'
void CPrintMonitoringDlg::BtnStartClick()
{
	btnStart.EnableWindow(FALSE);
	btnStop.EnableWindow(TRUE);
	cbPrinters.EnableWindow(FALSE);

	CString electPrinter;
	cbPrinters.GetWindowText(electPrinter);

	OnMonitor(electPrinter);
}

// Button 'stop'
void CPrintMonitoringDlg::BtnStopClick()
{
	btnStart.EnableWindow(TRUE);
	btnStop.EnableWindow(FALSE);
	cbPrinters.EnableWindow(TRUE);

	OffMonitor();
}

// Button `Collapse in tray '
void CPrintMonitoringDlg::OnBnClickedHide()
{
	// Add icons in tray
		memset(&nf, 0, sizeof(NOTIFYICONDATA));
		nf.cbSize = sizeof(NOTIFYICONDATA);
		nf.hWnd = m_hWnd;
		nf.uID = 1;
		nf.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nf.uCallbackMessage = WM_MYICONNOTIFY;
		nf.hIcon = AfxGetApp()->LoadIcon(IDI_PRINTERICON);

		lstrcpy(nf.szTip, L"Printer monitoring");
		Shell_NotifyIcon(NIM_ADD, &nf);

		ShowWindow(SW_HIDE);
}

// Intercepts clicks on the tray icon
LRESULT CPrintMonitoringDlg::OnIcon(WPARAM wp, LPARAM lp)
{
	if (lp == WM_LBUTTONDOWN){
		if (!IsWindowVisible())
		{
			nf.hWnd = m_hWnd;
			nf.uID = NULL;
			nf.uFlags = NIF_ICON;
			nf.uCallbackMessage = NULL;
			nf.hIcon = NULL;
			Shell_NotifyIcon(NIM_DELETE, &nf);

			ShowWindow(SW_RESTORE);
		}
		else{
			ShowWindow(SW_HIDE);
		}
	}

	if (lp == WM_RBUTTONDOWN) {
		CMenu oMenu, *pPopupMenu;
		oMenu.LoadMenu(IDR_MENU1);
		pPopupMenu = oMenu.GetSubMenu(0);
		if (pPopupMenu) {
			POINT pt;
			GetCursorPos(&pt);
			pPopupMenu->TrackPopupMenu(TPM_RIGHTALIGN, pt.x, pt.y, this);
		}
	}

	return 0;
}


void CPrintMonitoringDlg::OnBnClickedOpenlog()
{
	if(GetFileAttributes(L"log.txt") != DWORD(-1)){
		ShellExecute(NULL, NULL, L"log.txt", NULL, NULL, SW_RESTORE);
	}
	else
		MessageBox(L"File hasn`t created yet", L"Error", MB_ICONERROR);
}


void CPrintMonitoringDlg::OnBnClickedDeletelog()
{
	if (GetFileAttributes(L"log.txt") != DWORD(-1)) {
		remove("log.txt");
		logList.DeleteAllItems();
	}
	else
		MessageBox(L"File hasn`t created yet", L"Error", MB_ICONERROR);
}


void CPrintMonitoringDlg::OnBnClickedCbAutorun()
{
	autoRun = (CButton*)GetDlgItem(IDC_CB_AUTORUN);
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	HKEY hKey;

	if (autoRun->GetCheck() == BST_CHECKED) {
		if (RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey) == ERROR_SUCCESS) {
			RegSetValueEx(hKey, L"PrintMonitor", 0, REG_SZ, (LPBYTE)szPath, sizeof(szPath));
			RegCloseKey(hKey);
		}
	}

	else if(autoRun->GetCheck() == BST_UNCHECKED) {
		if (RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey) == ERROR_SUCCESS) {
			RegDeleteValue(hKey, L"PrintMonitor");
			RegCloseKey(hKey);
		}
	}
}
