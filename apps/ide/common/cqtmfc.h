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

#define _T(x) ((TCHAR*)(x))
#define TRACE0(x) qDebug(x);

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

#define RGB(r, g ,b)  ((DWORD) (((BYTE) (r) | \
    ((WORD) (g) << 8)) | \
    (((DWORD) (BYTE) (b)) << 16)))

#define RED(r) RGB(r,0,0)
#define GREEN(g) RGB(0,g,0)
#define BLUE(b) RGB(0,0,b)

#include <QMutex>
#include <QString>
#include <QFile>

class CSemaphore
{
};

class CString : public QString
{
public:
   CString()
   {
      clear();
   }

   CString(char* str)
   {
      clear();
      append(str);
   }

   void Format(const char* fmt, ...)
   {
      va_list argptr;
      va_start(argptr,fmt);
      sprintf(fmt,argptr);
      va_end(argptr);
   }
   void Format(TCHAR* fmt, ...)
   {
      va_list argptr;
      va_start(argptr,fmt);
      sprintf((char*)fmt,argptr);
      va_end(argptr);
   }

   CString& operator=(const char* str)
   {
      clear();
      append(str);
      return *this;
   }
   CString& operator=(TCHAR* str)
   {
      clear();
      append((char*)str);
      return *this;
   }
   CString& operator+=(TCHAR* str)
   {
      append((char*)str);
      return *this;
   }

   void Empty() { clear(); }
   const char* GetString() const
   {
      return toAscii().constData();
   }
   CString Left( int nCount ) const
   {
      return CString(left(nCount).toAscii().data());
   }
   CString Right( int nCount ) const
   {
      return CString(right(nCount).toAscii().data());
   }
   int GetLength() const
   {
      return length();
   }
   int CompareNoCase( TCHAR* lpsz ) const
   {
      return compare(QString((char*)lpsz),Qt::CaseInsensitive);
   }
   TCHAR GetAt( int nIndex ) const
   {
      return at(nIndex).toAscii();
   }
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
   CFile()
   {
      _qfile = NULL;
   }
   virtual ~CFile()
   {
      if ( _qfile )
         _qfile->close();
      delete _qfile;
   }

   virtual void Write(
      const void* lpBuf,
      UINT nCount
   )
   {
      if ( _qfile )
         _qfile->write((const char*)lpBuf,nCount);
   }
   virtual UINT Read(
      void* lpBuf,
      UINT nCount
   )
   {
      if ( _qfile )
         return _qfile->read((char*)lpBuf,nCount);
      else
         return 0;
   }
   virtual BOOL Open(
      LPCTSTR lpszFileName,
      UINT nOpenFlags,
      CFileException* pError = NULL
   )
   {
      _qfile = new QFile((char*)lpszFileName);
      QFile::OpenMode flags;
      _qfile->setFileName((char*)lpszFileName);
      if ( nOpenFlags&modeRead ) flags = QIODevice::ReadOnly;
      if ( nOpenFlags&modeWrite ) flags = QIODevice::WriteOnly;
      if ( nOpenFlags&modeCreate ) flags |= QIODevice::Truncate;
      _qfile->open(flags);
      return TRUE;
   }
   virtual ULONGLONG GetLength( ) const
   {
      return _qfile->size();
   }
   virtual void Close()
   {
      _qfile->close();
      delete _qfile;
      _qfile = NULL;
   }

private:
   QFile *_qfile;
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
#define ASSERT(y) { if (!(y)) qDebug("ASSERT!"); }
#endif

// Structures from files I didn't port verbatim.
// From PatternEditor.h
// Row color cache
struct RowColorInfo_t {
	COLORREF Note;
	COLORREF Instrument;
	COLORREF Volume;
	COLORREF Effect;
	COLORREF Back;
	COLORREF Shaded;
};

#endif // CQTMFC_H
