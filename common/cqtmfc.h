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
#include <QFrame>
#include <QComboBox>

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

#ifdef UNICODE
#define _T(x) L##x
#else
#define _T(x) x
#endif 
#define TRACE0(x) { QString str; str.sprintf("TRACE0: %s(%d): %s",__FILE__,__LINE__, (x)); qDebug(str.toAscii().constData()); }

#define RGB(r,g,b) ((COLORREF)((BYTE)(r)|((BYTE)(g) << 8)|((BYTE)(b) << 16)))

#include <QMutex>
#include <QString>
#include <QFile>

class CSemaphore
{
};

size_t strlen(const TCHAR* str);

class CString
{
public:
   CString();
   CString(const CString& ref);
   CString(char* str);
   CString(const char* str);
   CString(TCHAR* str);
   CString(const TCHAR* str);
   CString(QString str);
   virtual ~CString();

   void AppendFormat(const char* fmt, ...);
   void AppendFormatV(const char* fmt, va_list ap);
   void AppendFormat(LPCTSTR fmt, ...);
   void AppendFormatV(LPCTSTR fmt, va_list ap);
   void Format(const char* fmt, ...);
   void FormatV(const char* fmt, va_list ap);
   void Format(LPCTSTR fmt, ...);
   void FormatV(LPCTSTR fmt, va_list ap);

   CString& operator=(const char* str);
   CString& operator+=(const char* str);
   CString& operator=(TCHAR* str);
   CString& operator+=(TCHAR* str);
   CString& operator=(QString str);
   CString& operator+=(QString str);
   CString& operator=(CString str);
   CString& operator+=(CString str);
   bool operator==(const CString& str) const;
   operator const char*() const;
   operator const QString&() const;
   operator const TCHAR*() const;

   void Empty();
   const char* GetString() const;
   LPTSTR GetBuffer() const;
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

class CPoint : public tagPOINT
{
public:
   CPoint()
   {
      x = 0;
      y = 0;
   }
   CPoint(QPoint point)
   {
      x = point.x();
      y = point.y();
   }
   void SetPoint(int _x, int _y)
   {
      x = _x;
      y = _y;
   }

//   int x;
//   int y;
};

class CRect
{
public:
   CRect( ); 
   CRect( 
      int l, 
      int t, 
      int r, 
      int b  
   );
   CRect( 
      const RECT& srcRect  
   );
   CRect( 
      LPCRECT lpSrcRect  
   );
   CRect( 
      POINT point, 
      SIZE size  
   );
   CRect( 
      POINT topLeft, 
      POINT bottomRight  
   );
   int Width() const { return _rect.right-_rect.left; }
   int Height() const { return _rect.bottom-_rect.top; }
   operator LPRECT() const
   {
      return (RECT*)&_rect;
   }
   operator LPCRECT() const
   {
      return (const RECT*)&_rect;
   }
private:
   RECT _rect;
};

class CObject
{
public:
   CObject() {}
   virtual ~CObject() {}
   virtual void DeleteObject() {}
};

class CGdiObject : public CObject
{
public:
   CGdiObject() {}
   virtual ~CGdiObject() {}
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
   virtual ~CPen() {}
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
   CBitmap() {}
   virtual ~CBitmap() {}
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
   virtual ~CBrush() {}
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
   CFont() {}
   virtual ~CFont() {}
   operator QFont() const
   {
      return _qfont;
   }
   BOOL CreateFont(
      int nHeight,
      int nWidth,
      int nEscapement,
      int nOrientation,
      int nWeight,
      BYTE bItalic,
      BYTE bUnderline,
      BYTE cStrikeOut,
      BYTE nCharSet,
      BYTE nOutPrecision,
      BYTE nClipPrecision,
      BYTE nQuality,
      BYTE nPitchAndFamily,
      LPCTSTR lpszFacename 
   );
   BOOL CreateFontIndirect(
      const LOGFONT* lpLogFont 
   );
private:
   QFont _qfont;
};

class CRgn : public CGdiObject
{
public:
   CRgn() {}
   virtual ~CRgn() {}
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
   CDC()
   {
      _qwidget = NULL;
      _qpainter = NULL;
      _pen = NULL;
      _brush = NULL;
      _font = NULL;
      _bitmap = NULL;
      _rgn = NULL;   
      _gdiobject = NULL;
      _object = NULL;
      _lineOrg.x = 0;
      _lineOrg.y = 0;
      _bkColor = QColor(0,0,0);
      _bkMode = 0;
      _textColor = QColor(0,0,0);
      _windowOrg.x = 0;
      _windowOrg.y = 0;
   }
   CDC(QWidget* parent)
   {
      _qwidget = parent;
      _qpainter = NULL;
      _pen = NULL;
      _brush = NULL;
      _font = NULL;
      _bitmap = NULL;
      _rgn = NULL;   
      _gdiobject = NULL;
      _object = NULL;
      _lineOrg.x = 0;
      _lineOrg.y = 0;
      _bkColor = QColor(0,0,0);
      _bkMode = 0;
      _textColor = QColor(0,0,0);
      _windowOrg.x = 0;
      _windowOrg.y = 0;
   }

   void attach()
   {
      _qpainter = new QPainter(_qwidget);
   }
   
   void attach(QWidget* widget)
   {
      _qpainter = new QPainter(widget);
   }
   
   void detach()
   {
      delete _qpainter;
      _qpainter = NULL;
   }
   
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
      if ( _pen )
         _qpainter->setPen((QPen)(*_pen));
      return temp;
   }

   CBrush* SelectObject(
      CBrush* pBrush 
   )
   {
      CBrush* temp = _brush;
      _brush = pBrush;
      if ( _brush )      
         _qpainter->setBrush((QBrush)(*_brush));
      return temp;
   }
   virtual CFont* SelectObject(
      CFont* pFont 
   )
   {
      CFont* temp = _font;
      _font = pFont;
      if ( _font )
         _qpainter->setFont((QFont)(*_font));
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
      CGdiObject* temp = _gdiobject;
      _gdiobject = pObject;
      return temp;
   }   
   CObject* SelectObject(
      CObject* pObject
   )
   {
      CObject* temp = _object;
      _object = pObject;
      return temp;
   }   
   COLORREF SetBkColor( 
      COLORREF crColor  
   )
   {
      COLORREF old = _bkColor.red()|(_bkColor.green()<<8)|_bkColor.blue()<<16;
      _bkColor = QColor(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
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
      COLORREF old = _textColor.red()|(_textColor.green()<<8)|_textColor.blue()<<16;
      _textColor = QColor(GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
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
   QWidget*    _qwidget;
   QPainter*   _qpainter;
   CPen*       _pen;
   CBrush*     _brush;
   CFont*      _font;
   CBitmap*    _bitmap;
   CRgn*       _rgn;   
   CGdiObject* _gdiobject;
   CObject*    _object;
   CPoint      _lineOrg;
   QColor      _bkColor;
   int         _bkMode;
   QColor      _textColor;
   CPoint      _windowOrg;
};

class CPaintDC : public CDC
{
public:
   CPaintDC(QWidget* parent) : CDC(parent) {}
};

class CScrollBar
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

class CView;
class CDocument;
class CFrameWnd : public QWidget
{
public:
   CFrameWnd(QWidget* parent = 0) : QWidget(parent) {}
   virtual ~CFrameWnd() {}
   virtual CView* GetActiveView() = 0;
   virtual void SetMessageText(LPCTSTR fmt,...) { qDebug("SetMessageText"); }
   virtual CDocument* GetDocument() { return NULL; }
};

class CWnd : public QWidget
{
public:
   CWnd(QWidget* parent) : QWidget(parent) 
   {
      m_pFrameWnd = (CFrameWnd*)parent;
   }

   CFrameWnd* GetParentFrame() const { return m_pFrameWnd; } 
   void SetFocus(CWnd* p=0) { setFocus(); }
   void OnMouseMove(UINT,CPoint) {}
   void OnLButtonDblClk(UINT,CPoint) {}
   void OnLButtonUp(UINT,CPoint) {}
   void OnRButtonUp(UINT,CPoint) {}
   void OnSize(UINT nType, int cx, int cy) {}
   UINT SetTimer(void* id, UINT interval, void*);
   void KillTimer(void*, UINT id);
   void OnTimer(UINT timerId) {}
   void OnKeyDown(UINT,UINT,UINT) {}
   void OnSetFocus(CWnd*) {}
   void OnKillFocus(CWnd*) {}
   void OnVScroll(UINT,UINT,CScrollBar*) {}
   void OnHScroll(UINT,UINT,CScrollBar*) {}
   void OnUpdate(CWnd* p=0,UINT hint=0,CObject* o=0) { repaint(); }
   void Invalidate(BOOL bErase = TRUE) {}
   void RedrawWindow(LPCRECT rect=0,CRgn* rgn=0,UINT f=0) { repaint(); }
   
protected:
   static QMap<int,int> mfcToQtTimer;
   
private:
   CFrameWnd* m_pFrameWnd;
};

class CView : public CWnd
{
public:
   CView(QWidget* parent) : CWnd(parent) {}
   virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
   virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {}  
   virtual void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
   virtual void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {}  
};

class CDocument
{
};

class CComboBox : public QComboBox
{
public:
   void ResetContent();
   int AddString(CString& text);
   void SetCurSel(int sel);
};

class CWinApp
{
};

#define afx_msg 

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) > (b)) ? (b) : (a))

#ifdef QT_NO_DEBUG
#define ASSERT(y)
#define ASSERT_VALID(y)
#else
#define ASSERT(y) { if (!(y)) { QString str; str.sprintf("ASSERT: %s(%d)",__FILE__,__LINE__); qDebug(str.toAscii().constData()); } }
#define ASSERT_VALID(y) { if (!(y)) { QString str; str.sprintf("ASSERT: %s(%d)",__FILE__,__LINE__); qDebug(str.toAscii().constData()); } }
#endif

#endif // CQTMFC_H
