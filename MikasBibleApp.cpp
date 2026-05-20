
// MikasBibleApp.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MikasBibleApp.h"
#include "MainFrm.h"

#include "MikasBibleAppDoc.h"
#include "MikasBibleAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMikasBibleAppApp

BEGIN_MESSAGE_MAP(CMikasBibleAppApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMikasBibleAppApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CMikasBibleAppApp construction

CMikasBibleAppApp::CMikasBibleAppApp() noexcept
{

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MikasBibleApp.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CMikasBibleAppApp object

CMikasBibleAppApp theApp;


// CMikasBibleAppApp initialization

BOOL CMikasBibleAppApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	// Enable HTML Help (.chm) instead of legacy WinHelp.
	EnableHtmlHelp();

	// Load RichEdit 2.0 DLL (required by CMikasBibleAppView).
	AfxInitRichEdit2();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMikasBibleAppDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CMikasBibleAppView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Add "Help Topics" to the Help menu (menu is fully loaded at this point).
	CMenu* pMenu = m_pMainWnd->GetMenu();
	if (pMenu)
	{
		CMenu* pHelp = pMenu->GetSubMenu(pMenu->GetMenuItemCount() - 1);
		if (pHelp)
			pHelp->InsertMenu(0, MF_BYPOSITION | MF_STRING, ID_HELP_FINDER,
			                  _T("&Help Topics\tF1"));
		m_pMainWnd->DrawMenuBar();
	}

	return TRUE;
}

int CMikasBibleAppApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CMikasBibleAppApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	CStatic m_wndCopy;
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Expand the dialog downward to fit the copyright notice.
	CRect rcWin;
	GetWindowRect(&rcWin);
	SetWindowPos(nullptr, 0, 0, rcWin.Width(), rcWin.Height() + 48,
		         SWP_NOMOVE | SWP_NOZORDER);

	CRect rcClient;
	GetClientRect(&rcClient);

	m_wndCopy.Create(
		_T("Copyright \u00A9 Mika Huttunen, 2026\r\nJesus is Lord!"),
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		CRect(8, rcClient.bottom - 42, rcClient.right - 8, rcClient.bottom - 4),
		this);
	m_wndCopy.SetFont(GetFont());

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CMikasBibleAppApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMikasBibleAppApp message handlers



