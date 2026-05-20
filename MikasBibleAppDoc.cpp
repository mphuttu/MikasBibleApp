
// MikasBibleAppDoc.cpp : implementation of the CMikasBibleAppDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MikasBibleApp.h"
#endif

#include "MikasBibleAppDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMikasBibleAppDoc

IMPLEMENT_DYNCREATE(CMikasBibleAppDoc, CDocument)

BEGIN_MESSAGE_MAP(CMikasBibleAppDoc, CDocument)
END_MESSAGE_MAP()


// CMikasBibleAppDoc construction/destruction

CMikasBibleAppDoc::CMikasBibleAppDoc() noexcept
    : m_currentBookId(0), m_currentChapter(0)
{
}

CMikasBibleAppDoc::~CMikasBibleAppDoc()
{
    m_db.Close();
}

BOOL CMikasBibleAppDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // Open DB connection and load the book list (lightweight metadata only).
    if (!m_db.IsOpen())
    {
        if (!m_db.Open())
        {
            AfxMessageBox(
                _T("Could not connect to BibleDB.\n\n")
                _T("Please run ImportBible.ps1 first to create the database.\n\n")
                + m_db.GetLastError(),
                MB_OK | MB_ICONWARNING);
            return TRUE;   // continue without data
        }
    }

    m_books.clear();
    for (const BookRecord& br : m_db.GetBooks())
    {
        BookInfo bi{};
        bi.bookId       = br.bookId;
        bi.shortName    = br.shortName;
        bi.fullName     = br.fullName;
        bi.chapterCount = m_db.GetChapterCount(br.bookId);
        m_books.push_back(bi);
    }

    // Default selection: first book, first chapter.
    if (!m_books.empty())
    {
        m_currentBookId = m_books[0].bookId;
        m_currentChapter = 1;
    }

    return TRUE;
}

void CMikasBibleAppDoc::SelectChapter(int bookId, int chapter)
{
    m_currentBookId  = bookId;
    m_currentChapter = chapter;
    UpdateAllViews(nullptr, HINT_CHAPTER_SELECTED);
}

std::vector<VerseRecord> CMikasBibleAppDoc::GetCurrentVerses()
{
    if (!m_db.IsOpen() || m_currentBookId == 0)
        return {};
    return m_db.GetVerses(m_currentBookId, m_currentChapter);
}




// CMikasBibleAppDoc serialization

void CMikasBibleAppDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CMikasBibleAppDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CMikasBibleAppDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMikasBibleAppDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMikasBibleAppDoc diagnostics

#ifdef _DEBUG
void CMikasBibleAppDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMikasBibleAppDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMikasBibleAppDoc commands
