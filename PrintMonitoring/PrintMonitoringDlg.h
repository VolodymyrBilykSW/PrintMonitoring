
// PrintMonitoringDlg.h : файл заголовка
//
//---------------------------------------------------------------------------


// диалоговое окно CPrintMonitoringDlg
class CPrintMonitoringDlg : public CDialogEx
{
// Создание
public:
	CPrintMonitoringDlg(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRINTMONITORING_DIALOG };
#endif

	CButton	btnStop;
	CButton	btnStart;
	CButton	btnCancel;
	CButton *autoRun;
	CComboBox cbPrinters;

	// структура інформації в треї
	NOTIFYICONDATA nf;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCbnSelchangeCbPrinters();
	afx_msg void BtnStartClick();
	afx_msg void BtnStopClick();
	afx_msg void OnBnClickedHide();
	afx_msg LRESULT OnIcon(WPARAM wp, LPARAM lp);
	afx_msg void OnBnClickedOpenlog();
	afx_msg void OnBnClickedDeletelog();
	afx_msg void OnBnClickedAutorun();
	afx_msg void OnBnClickedCbAutorun();
};
