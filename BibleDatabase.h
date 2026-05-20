#pragma once
#include <sql.h>
#include <sqlext.h>
#include <vector>

// ── Data transfer structs ────────────────────────────────────────────────────

struct BookRecord
{
    int     bookId;
    CString shortName;
    CString fullName;
    int     bookOrder;
};

struct VerseRecord
{
    int     verseNum;
    CString text;
};

// ── CBibleDatabase ────────────────────────────────────────────────────────────
// Thin ODBC wrapper for the BibleDB SQL Server LocalDB database.
// Call Open() once (e.g. in CMikasBibleAppDoc::OnNewDocument).
// All query methods return empty collections on error; check GetLastError().

class CBibleDatabase
{
public:
    CBibleDatabase();
    ~CBibleDatabase();

    BOOL    Open();
    void    Close();
    BOOL    IsOpen() const { return m_bOpen; }

    // Returns all books ordered by BookOrder.
    std::vector<BookRecord> GetBooks();

    // Returns the highest chapter number for a book (= chapter count).
    int GetChapterCount(int bookId);

    // Returns all verses for a given book and chapter, ordered by VerseNum.
    std::vector<VerseRecord> GetVerses(int bookId, int chapter);

    CString GetLastError() const { return m_lastError; }

private:
    SQLHENV  m_hEnv;
    SQLHDBC  m_hDbc;
    BOOL     m_bOpen;
    CString  m_lastError;

    void     RecordOdbcError(SQLHANDLE handle, SQLSMALLINT handleType);
};
