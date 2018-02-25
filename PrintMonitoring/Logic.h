#pragma once

#include <windows.h>
#include <winspool.h>
#include <fstream>

using namespace std;


// Struct which contains information about print
struct PrintInfo
{
	LPCWSTR printerName;
	LPCWSTR userName;
	LPCWSTR computerName;
	LPCWSTR documentName;
	DWORD	pages;
	LPCWSTR time;
	LPCWSTR date;
	CString Cdate;
	CString Ctime;
};

HANDLE hStopEvent;	// finished monitoring event
CListCtrl logList;	// listBox for output data 
wofstream logFile;	// file stream for output data

// Loading available printer list
PRINTER_INFO_2* LoadPrinters(DWORD &pCount)
{
	// Initilization data
	BYTE *printerBuffer = NULL;
	DWORD bytesNeeded = 0;
	DWORD flags = PRINTER_ENUM_LOCAL | PRINTER_ENUM_NETWORK;
	DWORD level = 2;

	// Getting the required buffer length
	BOOL result = EnumPrinters(flags, NULL, level, NULL, 0, &bytesNeeded, &pCount);

	DWORD err = GetLastError();

	if (err != ERROR_INSUFFICIENT_BUFFER || bytesNeeded == 0)
	{
		MessageBox(NULL, (LPCWSTR)err, L"Buffer size failed", MB_ICONERROR);
		return false;
	}

	// Allocation of necessary memory
	printerBuffer = (BYTE *)malloc(bytesNeeded);

	if (printerBuffer == NULL)
	{
		MessageBox(NULL, L"malloc failed", L"ERROR", MB_ICONERROR);
		return false;
	}

	result = EnumPrinters(flags, NULL, level, printerBuffer, bytesNeeded, &bytesNeeded, &pCount);

	if (result == NULL)
	{
		MessageBox(NULL, (LPCWSTR)GetLastError(), L"EnumPrinters failed", MB_ICONERROR);
		return false;
	}

	PRINTER_INFO_2 *pInfoArray = (PRINTER_INFO_2 *)printerBuffer;
	return pInfoArray;
}

// Getting information about the printing
PrintInfo GetPrintInfo(PRINTER_NOTIFY_INFO *outData) {
	PrintInfo info = { 0 };

	// Getting information from each field
	for (DWORD i = 0; i < outData->Count; i++) {

		if (outData->aData[i].Type == JOB_NOTIFY_TYPE) {
			switch (outData->aData[i].Field) {

			case JOB_NOTIFY_FIELD_MACHINE_NAME: {
				info.computerName = (LPCWSTR)outData->aData[i].NotifyData.Data.pBuf;
				break;
			}

			case JOB_NOTIFY_FIELD_TOTAL_PAGES: {
				info.pages = outData->aData[i].NotifyData.adwData[0];
				break;
			}

			case JOB_NOTIFY_FIELD_DOCUMENT: {
				info.documentName = (LPCWSTR)outData->aData[i].NotifyData.Data.pBuf;
				break;
			}

			case JOB_NOTIFY_FIELD_PRINTER_NAME: {
				info.printerName = (LPCWSTR)outData->aData[i].NotifyData.Data.pBuf;
				break;
			}

			case JOB_NOTIFY_FIELD_USER_NAME: {
				info.userName = (LPCWSTR)outData->aData[i].NotifyData.Data.pBuf;
				break;
			}

			case JOB_NOTIFY_FIELD_SUBMITTED: {
				SYSTEMTIME time = *((SYSTEMTIME *)outData->aData[i].NotifyData.Data.pBuf);
				CString str;

				SystemTimeToTzSpecificLocalTime(NULL, &time, &time);
				str.Format(_T("%02u:%02u:%02u"), time.wHour, time.wMinute, time.wSecond);
				info.time = (LPCWSTR)str;
				info.Ctime = str;

				str.Format(_T("%02u.%02u.%4u"), time.wDay, time.wMonth, time.wYear);
				info.date = (LPCWSTR)str;
				info.Cdate = str;
				break;
			}

			}
		}
	}

	return info;
}

// Writting data printing to log file and to listBox
void WriteLog(PrintInfo data) 
{
	logFile.imbue(locale(""));

	logFile.open("log.txt", ios::app | ios::binary);
	logFile << "[" << data.date << "]" << data.time << "\t" << data.printerName << "\t";
	logFile << data.computerName << "\\" << data.userName << "\t" << data.documentName << "\t" << data.pages;
	logFile << "\r\n";

	logFile.close();


	// Writting to listControl
	CString str;
	int nItem = logList.InsertItem(0, data.date);

	logList.SetItemText(nItem, 1, data.time);
	logList.SetItemText(nItem, 2, data.printerName);
	logList.SetItemText(nItem, 3, data.documentName);

	str.Format(_T("%s\\%s"), data.computerName, data.userName);
	logList.SetItemText(nItem, 4, str);

	str.Format(_T("%d"), data.pages);
	logList.SetItemText(nItem, 5, str);
}

// Thread function for monitoring
BOOL WINAPI Proc(CString* name)
{
	LPWSTR printerName = (LPWSTR)(LPCWSTR)*name;
	HANDLE hPrinter = NULL;
	PRINTER_NOTIFY_INFO *Data = NULL;	// printer changes output
	PrintInfo last = { 0 };

	// Getting printer handle
	if (!OpenPrinter(printerName, &hPrinter, NULL))
	{
		MessageBox(NULL, L"Error! Couldn`t access to printer", printerName, MB_ICONERROR);
		_endthreadex(NULL);
		return FALSE;
	}

	// Fields from which information is taken
	WORD JobFields[] = {
		JOB_NOTIFY_FIELD_PRINTER_NAME,
		JOB_NOTIFY_FIELD_MACHINE_NAME,
		JOB_NOTIFY_FIELD_USER_NAME,
		JOB_NOTIFY_FIELD_DOCUMENT,
		JOB_NOTIFY_FIELD_SUBMITTED,
		JOB_NOTIFY_FIELD_TOTAL_PAGES,
	};
	PRINTER_NOTIFY_OPTIONS_TYPE	Notifications = {
			JOB_NOTIFY_TYPE,
			0,
			0,
			0,
			sizeof(JobFields) / sizeof(JobFields[0]),
			JobFields
	};
	PRINTER_NOTIFY_OPTIONS NotificationOptions = {
		2,
		PRINTER_NOTIFY_OPTIONS_REFRESH,
		1,
		&Notifications
	};


	// Getting a printer notification handle
	HANDLE hNotify = FindFirstPrinterChangeNotification(hPrinter, PRINTER_CHANGE_JOB, 0, &NotificationOptions);

	if (hNotify == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Error! Couldn`t access printer notifications", printerName, MB_ICONERROR);
		ClosePrinter(hPrinter);
		_endthreadex(NULL);
		return FALSE;
	}

	HANDLE hWait[2] = { hStopEvent, hNotify };

	while (true) {
		auto status = WaitForMultipleObjects(2, hWait, FALSE, INFINITE);
		// Waiting for signal state of thread
		if (status == WAIT_OBJECT_0) {
			// Monitoring was finished 
			break;
		}

		DWORD changes = 0;
		// Getting information about printer changes
		if (!FindNextPrinterChangeNotification(hNotify, &changes, &NotificationOptions, (LPVOID *)&Data)) {
			MessageBox(NULL, L"Couldn`t receive notification from printer", printerName, MB_ICONERROR);
			break;
		}

		if (changes == PRINTER_CHANGE_WRITE_JOB) {
			last = GetPrintInfo(Data);
		}

		if (changes == 0x600 && last.date != NULL) {
			WriteLog(last);
		}
	}

	// Closing a handle to monitor printer notifications
	FindClosePrinterChangeNotification(hNotify);

	// Closing printer handle
	ClosePrinter(hPrinter);

	MessageBox(NULL, L"Monitoring is complete", printerName, MB_OK);

	_endthreadex(NULL);
	return TRUE;
}


// Start monitoring on the specified printer, in a separate thread
void OnMonitor(CString printerName) {
	DWORD ThreadId;
	CString *parametr = new CString(printerName);

	hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Proc, (LPVOID)parametr, 0, &ThreadId);
}


// Stop monitoring
void OffMonitor() {
	SetEvent(hStopEvent);
}