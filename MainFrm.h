
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CSplitterWnd	  m_wndSplitter;

// Generated message map functions
protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHelpTopics();
	afx_msg void OnUpdateHelpTopics(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

};


