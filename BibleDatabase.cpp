#include "pch.h"
#include "BibleDatabase.h"

#pragma comment(lib, "odbc32.lib")

// Connection strings to try in order (newest driver first).
// The named pipe address is resolved automatically by LocalDB.
static const wchar_t* s_connStrings[] =
{
    L"Driver={ODBC Driver 18 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=BibleDB;Trusted_Connection=Yes;",
    L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=BibleDB;Trusted_Connection=Yes;",
    L"Driver={ODBC Driver 13 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=BibleDB;Trusted_Connection=Yes;",
    L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=BibleDB;Trusted_Connection=Yes;",
    nullptr
};

// ── Construction / destruction ───────────────────────────────────────────────

CBibleDatabase::CBibleDatabase()
    : m_hEnv(SQL_NULL_HENV), m_hDbc(SQL_NULL_HDBC), m_bOpen(FALSE)
{
}

CBibleDatabase::~CBibleDatabase()
{
    Close();
}

// ── Open / Close ─────────────────────────────────────────────────────────────

BOOL CBibleDatabase::Open()
{
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
    if (!SQL_SUCCEEDED(ret))
    {
        m_lastError = _T("Failed to allocate ODBC environment handle");
        return FALSE;
    }

    ret = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret))
    {
        RecordOdbcError(m_hEnv, SQL_HANDLE_ENV);
        return FALSE;
    }

    ret = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
    if (!SQL_SUCCEEDED(ret))
    {
        RecordOdbcError(m_hEnv, SQL_HANDLE_ENV);
        return FALSE;
    }

    // Try each driver in turn
    for (int i = 0; s_connStrings[i] != nullptr; ++i)
    {
        SQLWCHAR outBuf[1024] = {};
        SQLSMALLINT outLen = 0;
        ret = SQLDriverConnectW(m_hDbc, nullptr,
            (SQLWCHAR*)s_connStrings[i], SQL_NTS,
            outBuf, (SQLSMALLINT)_countof(outBuf), &outLen,
            SQL_DRIVER_NOPROMPT);
        if (SQL_SUCCEEDED(ret))
        {
            m_bOpen = TRUE;
            return TRUE;
        }
    }

    RecordOdbcError(m_hDbc, SQL_HANDLE_DBC);
    return FALSE;
}

void CBibleDatabase::Close()
{
    if (m_hDbc != SQL_NULL_HDBC)
    {
        SQLDisconnect(m_hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
        m_hDbc = SQL_NULL_HDBC;
    }
    if (m_hEnv != SQL_NULL_HENV)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
        m_hEnv = SQL_NULL_HENV;
    }
    m_bOpen = FALSE;
}

// ── Query helpers ────────────────────────────────────────────────────────────

std::vector<BookRecord> CBibleDatabase::GetBooks()
{
    std::vector<BookRecord> books;
    if (!m_bOpen) return books;

    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hStmt)))
        return books;

    static const wchar_t sql[] =
        L"SELECT BookId, ShortName, FullName, BookOrder "
        L"FROM Books ORDER BY BookOrder";

    if (SQL_SUCCEEDED(SQLExecDirectW(hStmt, (SQLWCHAR*)sql, SQL_NTS)))
    {
        while (SQLFetch(hStmt) == SQL_SUCCESS)
        {
            BookRecord rec{};
            SQLLEN     ind = 0;
            SQLINTEGER id = 0, order = 0;
            SQLWCHAR   shortName[32]  = {};
            SQLWCHAR   fullName[128]  = {};

            SQLGetData(hStmt, 1, SQL_C_LONG,  &id,        sizeof(id),        &ind);
            SQLGetData(hStmt, 2, SQL_C_WCHAR,  shortName,  sizeof(shortName), &ind);
            SQLGetData(hStmt, 3, SQL_C_WCHAR,  fullName,   sizeof(fullName),  &ind);
            SQLGetData(hStmt, 4, SQL_C_LONG,  &order,     sizeof(order),     &ind);

            rec.bookId    = (int)id;
            rec.shortName = CString(shortName);
            rec.fullName  = CString(fullName);
            rec.bookOrder = (int)order;
            books.push_back(rec);
        }
    }
    else
    {
        RecordOdbcError(hStmt, SQL_HANDLE_STMT);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return books;
}

int CBibleDatabase::GetChapterCount(int bookId)
{
    if (!m_bOpen) return 0;

    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hStmt)))
        return 0;

    // bookId is an internal integer – safe to embed directly.
    CString sql;
    sql.Format(_T("SELECT MAX(Chapter) FROM Verses WHERE BookId = %d"), bookId);

    int chapCount = 0;
    if (SQL_SUCCEEDED(SQLExecDirectW(hStmt, (SQLWCHAR*)(LPCWSTR)sql, SQL_NTS)))
    {
        if (SQLFetch(hStmt) == SQL_SUCCESS)
        {
            SQLLEN     ind = 0;
            SQLINTEGER maxChap = 0;
            SQLGetData(hStmt, 1, SQL_C_LONG, &maxChap, sizeof(maxChap), &ind);
            if (ind != SQL_NULL_DATA)
                chapCount = (int)maxChap;
        }
    }
    else
    {
        RecordOdbcError(hStmt, SQL_HANDLE_STMT);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return chapCount;
}

std::vector<VerseRecord> CBibleDatabase::GetVerses(int bookId, int chapter)
{
    std::vector<VerseRecord> verses;
    if (!m_bOpen) return verses;

    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hStmt)))
        return verses;

    CString sql;
    sql.Format(
        _T("SELECT VerseNum, VerseText FROM Verses ")
        _T("WHERE BookId = %d AND Chapter = %d ORDER BY VerseNum"),
        bookId, chapter);

    if (SQL_SUCCEEDED(SQLExecDirectW(hStmt, (SQLWCHAR*)(LPCWSTR)sql, SQL_NTS)))
    {
        while (SQLFetch(hStmt) == SQL_SUCCESS)
        {
            VerseRecord rec{};
            SQLLEN      ind = 0;
            SQLINTEGER  verseNum = 0;
            SQLWCHAR    verseText[2048] = {};

            SQLGetData(hStmt, 1, SQL_C_LONG,  &verseNum,  sizeof(verseNum),  &ind);
            SQLGetData(hStmt, 2, SQL_C_WCHAR,  verseText,  sizeof(verseText), &ind);

            rec.verseNum = (int)verseNum;
            rec.text     = CString(verseText);
            verses.push_back(rec);
        }
    }
    else
    {
        RecordOdbcError(hStmt, SQL_HANDLE_STMT);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return verses;
}

// ── Private helpers ──────────────────────────────────────────────────────────

void CBibleDatabase::RecordOdbcError(SQLHANDLE handle, SQLSMALLINT handleType)
{
    SQLWCHAR    state[8]    = {};
    SQLINTEGER  nativeErr   = 0;
    SQLWCHAR    msg[512]    = {};
    SQLSMALLINT msgLen      = 0;

    if (SQL_SUCCEEDED(SQLGetDiagRecW(handleType, handle, 1,
                                     state, &nativeErr, msg, (SQLSMALLINT)_countof(msg), &msgLen)))
    {
        m_lastError = CString(state) + _T(": ") + CString(msg);
    }
}
