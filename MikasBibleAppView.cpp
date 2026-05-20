
// MikasBibleAppView.cpp : implementation of the CMikasBibleAppView class
//

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MikasBibleApp.h"
#endif

#include "MikasBibleAppDoc.h"
#include "MikasBibleAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMikasBibleAppView

IMPLEMENT_DYNCREATE(CMikasBibleAppView, CView)

BEGIN_MESSAGE_MAP(CMikasBibleAppView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	// Forward focus to the rich-edit child so keyboard shortcuts work.
	ON_WM_SETFOCUS()
	// Copy support
	ON_COMMAND(ID_EDIT_COPY,                &CMikasBibleAppView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY,      &CMikasBibleAppView::OnUpdateEditCopy)
	// Printing (uses FormatRange on the rich-edit control)
	ON_COMMAND(ID_FILE_PRINT,               &CMikasBibleAppView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT,        &CMikasBibleAppView::OnFilePrint)
END_MESSAGE_MAP()

// CMikasBibleAppView construction/destruction

CMikasBibleAppView::CMikasBibleAppView() noexcept
{
}

CMikasBibleAppView::~CMikasBibleAppView()
{
}

BOOL CMikasBibleAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

int CMikasBibleAppView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the rich-edit control filling the entire client area.
	// ES_READONLY: user can select & copy text but not edit.
	CRect rc(0, 0, 0, 0);
	if (!m_richEdit.Create(
		WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
		rc, this, 1))
	{
		TRACE0("Failed to create rich-edit control\n");
		return -1;
	}

	// Use a readable serif font.
	LOGFONT lf{};
	lf.lfHeight = -16;
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = DEFAULT_CHARSET;
	_tcscpy_s(lf.lfFaceName, _T("Georgia"));
	m_richEdit.SetFont(CFont::FromHandle(CreateFontIndirect(&lf)));

	return 0;
}

void CMikasBibleAppView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if (m_richEdit.GetSafeHwnd())
		m_richEdit.MoveWindow(0, 0, cx, cy);
}

// CMikasBibleAppView drawing

void CMikasBibleAppView::OnDraw(CDC* /*pDC*/)
{
	// All drawing is handled by the rich-edit child window.
}

void CMikasBibleAppView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	DisplayCurrentChapter();
}

void CMikasBibleAppView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	if (lHint == HINT_CHAPTER_SELECTED || lHint == 0)
		DisplayCurrentChapter();
}

void CMikasBibleAppView::DisplayCurrentChapter()
{
	CMikasBibleAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc || !m_richEdit.GetSafeHwnd())
		return;

	m_richEdit.SetWindowText(_T(""));

	if (pDoc->m_books.empty())
	{
		m_richEdit.SetWindowText(_T("Database not loaded. Run ImportBible.ps1 first."));
		return;
	}

	// Find the current book's full name.
	CString bookTitle;
	for (const BookInfo& b : pDoc->m_books)
	{
		if (b.bookId == pDoc->m_currentBookId)
		{
			bookTitle = b.fullName;
			break;
		}
	}

	// Update the title bar: show book name instead of "Untitled".
	GetParentFrame()->SetWindowText(bookTitle);

	// Build the chapter header.
	CString header;
	header.Format(_T("%s  \u2013  Chapter %d\r\n\r\n"), (LPCTSTR)bookTitle, pDoc->m_currentChapter);

	// Start with a bold header via character formatting.
	m_richEdit.SetWindowText(_T(""));

	CHARFORMAT2 cf{};
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_BOLD | CFM_SIZE | CFM_FACE;
	cf.dwEffects = CFE_BOLD;
	cf.yHeight = 280;   // 14 pt in twips (1 pt = 20 twips)
	_tcscpy_s(cf.szFaceName, _T("Georgia"));

	m_richEdit.SetSel(0, -1);
	m_richEdit.SetSelectionCharFormat(cf);

	// Insert the header.
	m_richEdit.SetSel(-1, -1);
	m_richEdit.ReplaceSel(header);

	// Switch back to normal weight for the verse text.
	cf.dwEffects = 0;
	cf.yHeight   = 240;  // 12 pt
	long afterHeader = m_richEdit.GetWindowTextLength();
	m_richEdit.SetSel(afterHeader, afterHeader);
	m_richEdit.SetSelectionCharFormat(cf);

	// Insert verses.
	std::vector<VerseRecord> verses = pDoc->GetCurrentVerses();
	CString allText;
	for (const VerseRecord& v : verses)
	{
		CString line;
		line.Format(_T("%d  %s\r\n"), v.verseNum, (LPCTSTR)v.text);
		allText += line;
	}

	m_richEdit.SetSel(-1, -1);
	m_richEdit.ReplaceSel(allText);

	// Scroll back to the top.
	m_richEdit.SetSel(0, 0);
	m_richEdit.SendMessage(EM_SCROLLCARET);
}


// CMikasBibleAppView diagnostics

#ifdef _DEBUG
void CMikasBibleAppView::AssertValid() const
{
	CView::AssertValid();
}

void CMikasBibleAppView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMikasBibleAppDoc* CMikasBibleAppView::GetDocument() const // release version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMikasBibleAppDoc)));
	return (CMikasBibleAppDoc*)m_pDocument;
}
#endif //_DEBUG

void CMikasBibleAppView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	if (m_richEdit.GetSafeHwnd())
		m_richEdit.SetFocus();
}

void CMikasBibleAppView::OnEditCopy()
{
	m_richEdit.Copy();
}

void CMikasBibleAppView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_richEdit.GetSafeHwnd() &&
	               m_richEdit.GetSelectionType() != SEL_EMPTY);
}

// CMikasBibleAppView printing ────────────────────────────────────────────────
// Uses CRichEditCtrl::FormatRange so the full text is paginated correctly.

void CMikasBibleAppView::OnFilePrint()
{
	CPrintDialog dlg(FALSE);
	if (dlg.DoModal() != IDOK)
		return;

	CDC printerDC;
	printerDC.Attach(dlg.GetPrinterDC());

	// Physical page dimensions and resolution.
	int physW = printerDC.GetDeviceCaps(PHYSICALWIDTH);
	int physH = printerDC.GetDeviceCaps(PHYSICALHEIGHT);
	int offsX = printerDC.GetDeviceCaps(PHYSICALOFFSETX);
	int offsY = printerDC.GetDeviceCaps(PHYSICALOFFSETY);
	int dpiX  = printerDC.GetDeviceCaps(LOGPIXELSX);
	int dpiY  = printerDC.GetDeviceCaps(LOGPIXELSY);

	// Convert device pixels → twips (1 inch = 1440 twips).
	auto twX = [&](int px) -> long { return (long)((double)px * 1440.0 / dpiX); };
	auto twY = [&](int px) -> long { return (long)((double)px * 1440.0 / dpiY); };

	RECT rcPage{ 0, 0, twX(physW - 2 * offsX), twY(physH - 2 * offsY) };

	// 0.75-inch margins on all sides.
	const long margin = 1080;
	RECT rcRender = { rcPage.left + margin, rcPage.top + margin,
	                  rcPage.right - margin, rcPage.bottom - margin };

	// Document title shown in the print spooler.
	CMikasBibleAppDoc* pDoc = GetDocument();
	CString docTitle(_T("Bible"));
	if (pDoc)
	{
		for (const BookInfo& b : pDoc->m_books)
		{
			if (b.bookId == pDoc->m_currentBookId)
			{
				docTitle.Format(_T("%s, Chapter %d"),
				                (LPCTSTR)b.fullName, pDoc->m_currentChapter);
				break;
			}
		}
	}

	DOCINFO di{ sizeof(DOCINFO) };
	di.lpszDocName = docTitle;
	if (printerDC.StartDoc(&di) < 0)
		return;

	FORMATRANGE fr{};
	fr.hdc = fr.hdcTarget = printerDC.GetSafeHdc();
	fr.rcPage = rcPage;
	fr.rc     = rcRender;
	fr.chrg.cpMin = 0;
	fr.chrg.cpMax = -1;

	long textLen = m_richEdit.GetTextLength();
	if (textLen == 0)
	{
		printerDC.StartPage();
		printerDC.EndPage();
		printerDC.EndDoc();
		return;
	}

	printerDC.StartPage();
	while (fr.chrg.cpMin < textLen)
	{
		long next = m_richEdit.FormatRange(&fr, TRUE);
		if (next <= fr.chrg.cpMin)
			break;                        // safety – avoid infinite loop
		fr.chrg.cpMin = next;
		printerDC.EndPage();
		if (fr.chrg.cpMin < textLen)
		{
			printerDC.StartPage();
			fr.rc = rcRender;             // reset render rect for next page
		}
	}

	m_richEdit.FormatRange(nullptr, FALSE); // release internal cache
	printerDC.EndDoc();
}



