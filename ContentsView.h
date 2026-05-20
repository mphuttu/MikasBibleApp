#pragma once
#include <afxcview.h>

class CContentsView : public CTreeView
{
protected:
	CContentsView() noexcept;
	DECLARE_DYNCREATE(CContentsView)

public:
	virtual ~CContentsView();

protected:
	virtual void OnInitialUpdate() override;

	static LPARAM MakeItemData(int bookId, int chapter) { return ((LPARAM)bookId << 16) | (LPARAM)chapter; }
	static int    BookIdFromData(LPARAM d)   { return (int)(d >> 16); }
	static int    ChapterFromData(LPARAM d)  { return (int)(d & 0xFFFF); }

	CImageList m_imageList;   // index 0 = book icon, index 1 = chapter icon

	afx_msg void OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};


