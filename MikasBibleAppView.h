
// MikasBibleAppView.h : interface of the CMikasBibleAppView class
//

#pragma once
#include "MikasBibleAppDoc.h"

class CMikasBibleAppView : public CView
{
protected: // create from serialization only
	CMikasBibleAppView() noexcept;
	DECLARE_DYNCREATE(CMikasBibleAppView)

// Attributes
public:
	CMikasBibleAppDoc* GetDocument() const;

// Operations
public:
	// Render the current chapter from the document into the rich-edit control.
	void DisplayCurrentChapter();

// Overrides
public:
	virtual void OnDraw(CDC* pDC) override;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual void OnInitialUpdate() override;
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;

// Implementation
public:
	virtual ~CMikasBibleAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CRichEditCtrl m_richEdit;

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnFilePrint();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MikasBibleAppView.cpp
inline CMikasBibleAppDoc* CMikasBibleAppView::GetDocument() const
   { return reinterpret_cast<CMikasBibleAppDoc*>(m_pDocument); }
#endif

