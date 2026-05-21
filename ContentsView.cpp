#include "pch.h"
#include "ContentsView.h"
#include "MikasBibleAppDoc.h"

IMPLEMENT_DYNCREATE(CContentsView, CTreeView)

BEGIN_MESSAGE_MAP(CContentsView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CContentsView::OnTvnSelchanged)
END_MESSAGE_MAP()

CContentsView::CContentsView() noexcept {}
CContentsView::~CContentsView() {}

void CContentsView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	CTreeCtrl& tree = GetTreeCtrl();
	tree.DeleteAllItems();

	// ── Build image list from res\ folder ────────────────────────────────────
	// Installed: exe is in {app}\, icons are in {app}\res\
	// Development: exe is in x64\Debug\ or x64\Release\, res\ is two levels up.
	TCHAR exePath[MAX_PATH] = {};
	GetModuleFileName(nullptr, exePath, MAX_PATH);
	TCHAR* pSlash = _tcsrchr(exePath, _T('\\'));
	if (pSlash) *pSlash = _T('\0');
	CString exeDir = CString(exePath);
	CString resDir = exeDir + _T("\\res\\");
	// Fall back to development layout if installed res\ doesn't exist
	if (GetFileAttributes(resDir + _T("BibleBookIcon.ico")) == INVALID_FILE_ATTRIBUTES)
		resDir = exeDir + _T("\\..\\..\\res\\");

	bool iconsLoaded = false;
	if (m_imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 0))
	{
		HICON hBook = (HICON)LoadImage(nullptr,
			resDir + _T("BibleBookIcon.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		HICON hChap = (HICON)LoadImage(nullptr,
			resDir + _T("BibleChapterIcon.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

		if (hBook && hChap)
		{
			m_imageList.Add(hBook);   // index 0 → book
			m_imageList.Add(hChap);   // index 1 → chapter
			tree.SetImageList(&m_imageList, TVSIL_NORMAL);
			iconsLoaded = true;
		}
		if (hBook) DestroyIcon(hBook);
		if (hChap) DestroyIcon(hChap);
	}

	CMikasBibleAppDoc* pDoc = static_cast<CMikasBibleAppDoc*>(GetDocument());
	if (!pDoc || pDoc->m_books.empty())
		return;

	for (const BookInfo& book : pDoc->m_books)
	{
		HTREEITEM hBook;
		if (iconsLoaded)
			hBook = tree.InsertItem(book.fullName, 0, 0, TVI_ROOT, TVI_LAST);
		else
			hBook = tree.InsertItem(book.fullName, TVI_ROOT, TVI_LAST);
		tree.SetItemData(hBook, MakeItemData(book.bookId, 0));

		for (int ch = 1; ch <= book.chapterCount; ++ch)
		{
			CString label;
			label.Format(_T("Chapter %d"), ch);
			HTREEITEM hChap;
			if (iconsLoaded)
				hChap = tree.InsertItem(label, 1, 1, hBook, TVI_LAST);
			else
				hChap = tree.InsertItem(label, hBook, TVI_LAST);
			tree.SetItemData(hChap, MakeItemData(book.bookId, ch));
		}
	}

	// Expand and select the first book's first chapter.
	HTREEITEM hFirst = tree.GetRootItem();
	if (hFirst)
	{
		tree.Expand(hFirst, TVE_EXPAND);
		HTREEITEM hFirstChap = tree.GetChildItem(hFirst);
		if (hFirstChap)
			tree.SelectItem(hFirstChap);
	}
}

void CContentsView::OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	LPARAM       data  = pNMTV->itemNew.lParam;

	int bookId  = BookIdFromData(data);
	int chapter = ChapterFromData(data);

	// Only react when a chapter node is selected (chapter > 0).
	if (chapter > 0)
	{
		CMikasBibleAppDoc* pDoc = static_cast<CMikasBibleAppDoc*>(GetDocument());
		if (pDoc)
			pDoc->SelectChapter(bookId, chapter);
	}

	*pResult = 0;
}

