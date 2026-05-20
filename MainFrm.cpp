
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "MikasBibleApp.h"
#include <htmlhelp.h>
#pragma comment(lib, "htmlhelp.lib")

#include "MainFrm.h"
#include "MikasBibleAppView.h"
#include "ContentsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_HELP_FINDER,           &CMainFrame::OnHelpTopics)
	ON_UPDATE_COMMAND_UI(ID_HELP_FINDER, &CMainFrame::OnUpdateHelpTopics)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// Remove toolbar buttons not applicable to a read-only Bible reader.
	// (New / Open file actions and Paste editing have no function here.)
	{
		static const UINT nRemove[] = { ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_EDIT_CUT, ID_EDIT_PASTE };
		for (UINT nID : nRemove)
		{
			int nIdx = m_wndToolBar.CommandToIndex(nID);
			if (nIdx >= 0)
				m_wndToolBar.GetToolBarCtrl().DeleteButton(nIdx);
		}
		// Remove leading, trailing, and consecutive separators that are now orphaned.
		CToolBarCtrl& tbc = m_wndToolBar.GetToolBarCtrl();
		for (int i = tbc.GetButtonCount() - 1; i >= 0; --i)
		{
			TBBUTTON btn{};
			tbc.GetButton(i, &btn);
			if (!(btn.fsStyle & TBSTYLE_SEP)) continue;
			bool isEdge = (i == 0) || (i == tbc.GetButtonCount() - 1);
			bool nextIsSep = false;
			if (i < tbc.GetButtonCount() - 1)
			{
				TBBUTTON next{};
				tbc.GetButton(i + 1, &next);
				nextIsSep = (next.fsStyle & TBSTYLE_SEP) != 0;
			}
			if (isEdge || nextIsSep)
				tbc.DeleteButton(i);
		}
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// create a splitter with 1 row and 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;
	
	// Vasen näkymä: CTreeView-luokan aliluokka, joka näyttää Raamatun sisällysluettelon
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CContentsView), CSize(200, 0), pContext))
		return FALSE;
	// Oikea näkymä: CMikasBibleAppView-luokan aliluokka, joka näyttää Raamatun tekstin
	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CMikasBibleAppView), CSize(0, 0), pContext))
		return FALSE;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnHelpTopics()
{
	// Build the full path to MikasBibleApp.chm next to the running executable.
	TCHAR exePath[MAX_PATH];
	GetModuleFileName(nullptr, exePath, MAX_PATH);
	TCHAR* pSlash = _tcsrchr(exePath, _T('\\'));
	if (pSlash) pSlash[1] = _T('\0');
	CString chm = CString(exePath) + _T("MikasBibleApp.chm");

	HWND hWnd = ::HtmlHelp(GetSafeHwnd(), chm, HH_DISPLAY_TOC, 0);
	if (!hWnd)
		AfxMessageBox(_T("Help file not found.\n\nPlease place MikasBibleApp.chm in the same folder as MikasBibleApp.exe."),
		              MB_ICONINFORMATION);
}

void CMainFrame::OnUpdateHelpTopics(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
