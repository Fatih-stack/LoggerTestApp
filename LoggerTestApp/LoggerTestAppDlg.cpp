
// LoggerTestAppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "LoggerTestApp.h"
#include "LoggerTestAppDlg.h"
#include "afxdialogex.h"
#include "Logger.h"
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <iomanip>

class Timer
{
public:
	static double result;
	//initialize class with a test name and by starting timer
	Timer() : m_Stop(false)
	{
		m_BeginTime = std::chrono::high_resolution_clock::now();
	}

	//destructor if timer not stopped stop timer
	~Timer() { if (!m_Stop) Stop(); }

	//calculate beginning and finishing times of timer and also get thread id
	//by using these calculations adds new profile to json file and stop timer
	void Stop()
	{
		auto endTime = std::chrono::high_resolution_clock::now();
		begin = std::chrono::time_point_cast<std::chrono::microseconds>(m_BeginTime).time_since_epoch().count();
		end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

		threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
		m_Stop = true;
		long long duration = end - begin;
		Timer::result = duration * 0.001;
	}
public:
	long long begin;
	long long end;
	uint32_t threadID;

private:
	std::chrono::time_point<std::chrono::steady_clock> m_BeginTime;
	bool m_Stop;
};

// CLoggerTestAppDlg dialog

CLoggerTestAppDlg::CLoggerTestAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGGERTESTAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLoggerTestAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditList);
}

BEGIN_MESSAGE_MAP(CLoggerTestAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CLoggerTestAppDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CLoggerTestAppDlg message handlers

BOOL CLoggerTestAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_MAXIMIZE);

	ShowWindow(SW_MINIMIZE);

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLoggerTestAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLoggerTestAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

double Timer::result = 0;

void CLoggerTestAppDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CFile FileObj;		//Variable for file operations
	std::wstring sFilePath;
	const TCHAR szFilter[] = _T("TXT Files (*.txt)|*.txt|All Files (*.*)|*.*||");
	CFileDialog dlg(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY, szFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		sFilePath = dlg.GetPathName();		//take filepath with filename
	}
	else {
		return;
	}
	std::string res(sFilePath.begin(), sFilePath.end());
	aricanli::general::Logger::instance().create_open_file(sFilePath);
	{
		Timer timer;
		aricanli::general::Logger::instance().SetPriority(aricanli::general::Severity::Trace);
		for(int i=0; i<100'000;i++)
			aricanli::general::log_test(i);

	}
	aricanli::general::Logger::instance().~Logger();
	CStdioFile fp;
	CString m_Buffer;
	CString m_TempBuffer;
	LPCTSTR str = sFilePath.c_str();
	fp.Open(str, CFile::modeRead);

	while (!feof(fp.m_pStream))
	{
		fp.ReadString(m_TempBuffer);
		m_Buffer += m_TempBuffer;
		m_Buffer += "\r\n";
	}

	fp.Close();
	
	
	std::ostringstream ost;
	ost << std::setprecision(8) << Timer::result;
	m_Buffer += ost.str().c_str();
	m_Buffer += " ms \r\n";
	m_EditList.SetWindowText(m_Buffer);
}
