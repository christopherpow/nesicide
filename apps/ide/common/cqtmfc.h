#ifndef CQTMFC_H
#define CQTMFC_H

#include <QObject>

#include <QDebug>

// get rid of MFC crap
// Releasing pointers
#define SAFE_RELEASE(p) \
        if (p != NULL) { \
                delete p;       \
                p = NULL;       \
        }       \

#define SAFE_RELEASE_ARRAY(p) \
        if (p != NULL) { \
                delete [] p;    \
                p = NULL;       \
        }       \

// MFC "replacements" (so I don't have to change FamiTracker code I don't want to change...)
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef long unsigned int DWORD;
typedef DWORD COLORREF;
typedef wchar_t WCHAR;
typedef WCHAR TCHAR;
typedef const TCHAR *LPCTSTR;
typedef TCHAR *LPTSTR;
typedef int WINBOOL;
typedef WINBOOL BOOL;
typedef unsigned int UINT;
typedef unsigned int *PUINT;
typedef unsigned long long ULONGLONG;

#define _T(x) (LPCTSTR)x
#define TRACE0(x) { QString str; str.sprintf("TRACE0: %s(%d): %s",__FILE__,__LINE__, (x)); qDebug(str.toAscii().constData()); }

#define RGB(r,g,b) ((COLORREF)((BYTE)(r)|((BYTE)(g) << 8)|((BYTE)(b) << 16)))

#define RED(r) RGB(r,0,0)
#define GREEN(g) RGB(0,g,0)
#define BLUE(b) RGB(0,0,b)

#include <QMutex>
#include <QString>
#include <QFile>

class CSemaphore
{
};

class CString
{
public:
   CString();
   CString(const CString& ref);
   CString(char* str);
   CString(const char* str);
   CString(TCHAR* str);
   CString(QString str);
   virtual ~CString();

   void Format(const char* fmt, ...);
   void FormatV(const char* fmt, va_list ap);
   void Format(LPCTSTR fmt, ...);
   void FormatV(LPCTSTR fmt, va_list ap);

   CString& operator=(const char* str);
   CString& operator=(TCHAR* str);
   CString& operator+=(TCHAR* str);
   CString& operator=(QString str);
   CString& operator+=(QString str);
   CString& operator=(CString str);
   CString& operator+=(CString str);
   bool operator==(const CString& str) const;
   operator const char*();
   operator QString();

   void Empty();
   const char* GetString() const;
   LPTSTR GetBuffer();
   CString Left( int nCount ) const;
   CString Right( int nCount ) const;
   int GetLength() const;
   int CompareNoCase( LPCTSTR lpsz ) const;
   TCHAR GetAt( int nIndex ) const;
   
private:
   QString _qstr;
};

class CEdit
{
};

class CFileException
{
public:
   virtual BOOL GetErrorMessage(
      LPTSTR lpszError,
      UINT nMaxError,
      PUINT pnHelpContext = NULL
   ) const
   {
      lpszError[0] = 0;
      return false;
   }
};

class CFile
{
public:
   enum
   {
      modeCreate = 0x01,
      modeRead = 0x02,
      modeWrite = 0x04,
      shareDenyWrite = 0x08
   };
   CFile();
   CFile(CString& lpszFileName, int nOpenFlags);
   virtual ~CFile();

   virtual void Write(
      const void* lpBuf,
      UINT nCount
   );
   virtual UINT Read(
      void* lpBuf,
      UINT nCount
   );
   virtual BOOL Open(
      LPCTSTR lpszFileName,
      UINT nOpenFlags,
      CFileException* pError = NULL
   );
   virtual ULONGLONG GetLength( ) const;
   virtual void Close();

private:
   QFile _qfile;
};

class CDC
{
};

class CCriticalSection
{
public:
   void Lock() {}
   void Unlock() {}
};

class CMutex : public QMutex
{
public:
   void Lock() { lock(); }
   void Unlock() { unlock(); }
};

#define LIMIT(v, max, min) v = ((v > max) ? max : ((v < min) ? min : v));//  if (v > max) v = max; else if (v < min) v = min;

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) > (b)) ? (b) : (a))

#ifdef QT_NO_DEBUG
#define ASSERT(y)
#else
#define ASSERT(y) { if (!(y)) { QString str; str.sprintf("ASSERT: %s(%d)",__FILE__,__LINE__); qDebug(str.toAscii().constData()); } }
#endif

// Structures from files I didn't port verbatim.
// From PatternEditor.h
#include "famitracker/PatternData.h"

// Structure used by clipboard, no pointers allowed here
struct stClipData {
	int	Channels;		// Number of channels copied
	int Rows;			// Number of rows copied
	int StartColumn;	// Start column in first channel
	int EndColumn;		// End column in last channel
	stChanNote Pattern[MAX_CHANNELS][MAX_PATTERN_LENGTH];
};

// Row color cache
struct RowColorInfo_t {
	COLORREF Note;
	COLORREF Instrument;
	COLORREF Volume;
	COLORREF Effect;
	COLORREF Back;
	COLORREF Shaded;
};

// Cursor position
class CCursorPos {
public:
	CCursorPos();
	CCursorPos(int Row, int Channel, int Column);
	const CCursorPos& operator=(const CCursorPos &pos);
	bool Invalid() const;

public:
	int m_iRow;
	int m_iColumn;
	int m_iChannel;
};

// Selection
class CSelection {
public:
	int GetRowStart() const;
	int GetRowEnd() const;
	int GetColStart() const;
	int GetColEnd() const;
	int GetChanStart() const;
	int GetChanEnd() const;
	bool IsWithin(CCursorPos pos) const;
	bool IsSingleChannel() const;

	void SetStart(CCursorPos pos);
	void SetEnd(CCursorPos pos);

public:
	CCursorPos m_cpStart;
	CCursorPos m_cpEnd;
};

#endif // CQTMFC_H
