#ifndef CQTMFC_H
#define CQTMFC_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QPen>
#include <QBrush>
#include <QBitmap>
#include <QFont>
#include <QRegion>

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

#include <windows.h>

class CPoint// : public tagPOINT
{
public:
   int x;
   int y;
};

class CGdiObject
{
};

// Pen styles [MFC value doesn't matter]
//enum 
//{
//   PS_SOLID,
//   PS_DASH,
//   PS_DOT,
//   PS_DASHDOT,
//   PS_DASHDOTDOT,   
//   PS_NULL,
//   PS_INSIDEFRAME
//};

class CPen : public CGdiObject
{
public:
   CPen( );
   CPen(
      int nPenStyle,
      int nWidth,
      COLORREF crColor 
   );
   CPen(
      int nPenStyle,
      int nWidth,
      const LOGBRUSH* pLogBrush,
      int nStyleCount = 0,
      const DWORD* lpStyle = NULL 
   );
   operator QPen() const
   {
      return _qpen;
   }

private:
   QPen _qpen;
};

class CBitmap : public CGdiObject
{
public:
   operator QBitmap() const
   {
      return _qbitmap;
   }
private:
   QBitmap _qbitmap;
};

class CBrush : public CGdiObject
{
public:
   CBrush( );
   CBrush(
      COLORREF crColor 
   );
   CBrush(
      int nIndex,
      COLORREF crColor 
   );
   explicit CBrush(
      CBitmap* pBitmap 
   );
   operator QBrush() const
   {
      return _qbrush;
   }
private:
   QBrush _qbrush;
};

class CFont : public CGdiObject
{
public:
   operator QFont() const
   {
      return _qfont;
   }
private:
   QFont _qfont;
};

class CRgn : public CGdiObject
{
public:
   operator QRegion() const
   {
      return _qregion;
   }
private:
   QRegion _qregion;
};

class CDC
{
public:
   CDC(QWidget* parent);
   virtual ~CDC();
   BOOL BitBlt(
      int x,
      int y,
      int nWidth,
      int nHeight,
      CDC* pSrcDC,
      int xSrc,
      int ySrc,
      DWORD dwRop 
   );
   void Draw3dRect( LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight )
   {
      Draw3dRect(lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top,clrTopLeft,clrBottomRight);
   }
   void Draw3dRect( int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight );
   int DrawText(
      const CString& str,
      LPRECT lpRect,
      UINT nFormat 
   );
   void FillSolidRect(
      LPCRECT lpRect,
      COLORREF clr 
   );
   void FillSolidRect(
      int x,
      int y,
      int cx,
      int cy,
      COLORREF clr 
   );
   int GetDeviceCaps(
      int nIndex 
   ) const
   {
      return 0;
   }
   BOOL GradientFill( 
      TRIVERTEX* pVertices, 
      ULONG nVertices, 
      void* pMesh, 
      ULONG nMeshElements, 
      DWORD dwMode  
   );   
   BOOL LineTo( 
      int x, 
      int y  
   ); 
   BOOL LineTo( 
      POINT point  
   )
   {
      return LineTo(point.x,point.y);
   }

   CPoint MoveTo( 
      int x, 
      int y  
   )
   {
      CPoint old = _lineOrg;
      _lineOrg.x = x;
      _lineOrg.y = y;
      return old;
   }
   CPoint MoveTo( 
      POINT point  
   )
   {
      return MoveTo(point.x,point.y);
   }

   CPoint OffsetWindowOrg( 
      int nWidth, 
      int nHeight  
   )
   {
      CPoint old = _windowOrg;
      _windowOrg.x += nWidth;
      _windowOrg.y += nHeight;
      return old;
   }
   BOOL Polygon(
      LPPOINT lpPoints,
      int nCount 
   );   

   CPen* SelectObject(
      CPen* pPen 
   )
   {
      CPen* temp = _pen;
      _pen = pPen;
      _qpainter->setPen((QPen)(*_pen));
      return temp;
   }

   CBrush* SelectObject(
      CBrush* pBrush 
   )
   {
      CBrush* temp = _brush;
      _brush = pBrush;
      return temp;
   }
   virtual CFont* SelectObject(
      CFont* pFont 
   )
   {
      CFont* temp = _font;
      _font = pFont;
      return temp;
   }
   CBitmap* SelectObject(
      CBitmap* pBitmap 
   )
   {
      CBitmap* temp = _bitmap;
      _bitmap = pBitmap;
      return temp;
   }   
   int SelectObject(
      CRgn* pRgn 
   );
   CGdiObject* SelectObject(
      CGdiObject* pObject
   )
   {
      CGdiObject* temp = _object;
      _object = pObject;
      return temp;
   }   
   COLORREF SetBkColor( 
      COLORREF crColor  
   )
   {
      COLORREF old = _bkColor;
      _bkColor = crColor;
      return old;
   }
   int SetBkMode( 
      int nBkMode  
   )
   {
      int old = _bkMode;
      _bkMode = nBkMode;
      return old;
   }
   COLORREF SetPixel( int x, int y, COLORREF crColor );
   COLORREF SetPixel( POINT point, COLORREF crColor )
   {
      return SetPixel(point.x,point.y,crColor);
   }

   COLORREF SetTextColor( 
      COLORREF crColor  
   )
   {
      COLORREF old = _textColor;
      _textColor = crColor;
      return old;
   }
   CPoint SetWindowOrg(
      int x,
      int y 
   )
   {
      CPoint old = _windowOrg;
      _windowOrg.x = x;
      _windowOrg.y = y;
      return old;
   }
   CPoint SetWindowOrg(
      POINT point 
   )
   {
      return SetWindowOrg(point.x,point.y);
   }

   BOOL TextOut(
      int x,
      int y,
      LPCTSTR lpszString,
      int nCount 
   );
   BOOL TextOut(
      int x,
         int y,
         const CString& str 
   );
   
private:
   CDC(CDC& orig);
   QPainter*   _qpainter;
   CPen*       _pen;
   CBrush*     _brush;
   CFont*      _font;
   CBitmap*    _bitmap;
   CRgn*       _rgn;   
   CGdiObject* _object;
   CPoint      _lineOrg;
   COLORREF    _bkColor;
   int         _bkMode;
   COLORREF    _textColor;
   CPoint      _windowOrg;
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
