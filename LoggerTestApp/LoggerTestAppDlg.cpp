
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
	aricanli::general::Logger::instance().enable_file_output(res);

	std::thread threads[3];
	for (int i = 0; i < 3; i++)
		threads[i] = std::thread(aricanli::general::log_test, i);

	for (int i = 0; i < 3; i++)
		threads[i].join();
	aricanli::general::Logger::instance().close_file();

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
	m_EditList.SetWindowText(m_Buffer);
}
