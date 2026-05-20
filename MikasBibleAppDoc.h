
// MikasBibleAppDoc.h : interface of the CMikasBibleAppDoc class
//
#pragma once
#include <vector>
#include "BibleDatabase.h"

// Hint passed to UpdateAllViews when the user selects a chapter.
#define HINT_CHAPTER_SELECTED 1L

struct BookInfo
{
	int     bookId;
	CString shortName;
	CString fullName;
	int     chapterCount;
};


class CMikasBibleAppDoc : public CDocument
{
protected: // create from serialization only
	CMikasBibleAppDoc() noexcept;
	DECLARE_DYNCREATE(CMikasBibleAppDoc)

// Attributes
public:
	CBibleDatabase          m_db;
	std::vector<BookInfo>   m_books;   // loaded once from DB on startup

	int  m_currentBookId;
	int  m_currentChapter;

	// Called by CContentsView when the user picks a chapter.
	void SelectChapter(int bookId, int chapter);

	// Returns verses for the currently selected book/chapter.
	std::vector<VerseRecord> GetCurrentVerses();

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CMikasBibleAppDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
