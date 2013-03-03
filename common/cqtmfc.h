#ifndef CQTMFC_H
#define CQTMFC_H

#include <QApplication>
#include <QAction>
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
#include <QClipboard>
#include <QScrollBar>
#include <QEvent>
#include <QList>
#include <QMap>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QObject>
#include <QThread>
#include <QFile>
#include <QMutex>
#include <QByteArray>
#include <QGridLayout>
#include <QMimeData>
#include <QElapsedTimer>
#include <QSharedMemory>
#include <QSettings>
#include <QMutex>
#include <QSemaphore>
#include <QListWidget>
#include <QDialog>
#include <QMenu>

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


#if defined(Q_WS_WIN) || defined(Q_WS_WIN32)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) > (b)) ? (b) : (a))
#endif

// workaround to force ignore ms_abi errors, not needed as long as we don't link with other mfc implementations
#if !defined(Q_WS_WIN) && !defined(Q_WS_WIN32)
#if !__has_attribute(ms_abi)
#define ms_abi
#endif
#endif

#include <windows.h>

#ifdef UNICODE
#define _T(x) L##x
#else
#define _T(x) x
#endif 
#if !defined(TRACE0)
#define TRACE0(x) { QString str; str.sprintf("TRACE0: %s(%d): %s",__FILE__,__LINE__, (x)); qDebug(str.toLatin1().constData()); }
#endif
#if !defined(TRACE)
#define TRACE(x) { QString str; str.sprintf("TRACE0: %s(%d): %s",__FILE__,__LINE__, (x)); qDebug(str.toLatin1().constData()); }
#endif
#if !defined(ATLTRACE2)
#define ATLTRACE2(a,b,str,...)
#endif

#define POSITION int

#define DECLARE_DYNCREATE(x) 
#define DECLARE_MESSAGE_MAP()
#define DECLARE_DYNAMIC(x)
#define IMPLEMENT_DYNAMIC(x,y)

#define IDR_MAINFRAME 0xDEADBEEF
#define RUNTIME_CLASS(x) new x

#define IDD_INSTRUMENT              100000
#define IDD_INSTRUMENT_INTERNAL     100001
#define IDD_INSTRUMENT_DPCM         100002
#define IDD_INSTRUMENT_VRC7         100003
#define IDD_INSTRUMENT_FDS          100004
#define IDD_INSTRUMENT_FDS_ENVELOPE 100005
#define IDD_INSTRUMENT_N163_WAVE    100006

#define afx_msg

#ifdef QT_NO_DEBUG
#define ASSERT(y)
#define ASSERT_VALID(y)
#else
#define ASSERT(y) { if (!(y)) { QString str; str.sprintf("ASSERT: %s(%d)",__FILE__,__LINE__); qDebug(str.toLatin1().constData()); } }
#define ASSERT_VALID(y) { if (!(y)) { QString str; str.sprintf("ASSERT: %s(%d)",__FILE__,__LINE__); qDebug(str.toLatin1().constData()); } }
#endif

size_t strlen(const wchar_t* str);

int MulDiv(
  int nNumber,
  int nNumerator,
  int nDenominator
);
DWORD WINAPI GetTickCount(void);
DWORD WINAPI GetSysColor(
  int nIndex
);
int WINAPI GetSystemMetrics(
  int nIndex
);
BOOL WINAPI IsClipboardFormatAvailable(
  UINT format
);
BOOL WINAPI OpenClipboard(
  HWND hWndNewOwner = 0
);
BOOL WINAPI EmptyClipboard(void);
BOOL WINAPI CloseClipboard(void);
HANDLE WINAPI SetClipboardData(
  UINT uFormat,
  HANDLE hMem
);
HANDLE WINAPI GetClipboardData(
  UINT uFormat
);
HGLOBAL WINAPI GlobalAlloc(
  UINT uFlags,
  SIZE_T dwBytes
);
LPVOID WINAPI GlobalLock(
  HGLOBAL hMem
);
BOOL WINAPI GlobalUnlock(
  HGLOBAL hMem
);
SIZE_T WINAPI GlobalSize(
  HGLOBAL hMem
);

class CObject
{
public:
   CObject() {}
   virtual ~CObject() {}
   virtual void DeleteObject() {}
};

class CCriticalSection
{
public:
   BOOL Lock() { return TRUE; }
   BOOL Unlock() { return TRUE; }
};

class CMutex : public QMutex
{
public:
   void Lock() { lock(); }
   void Unlock() { unlock(); }
};

class CSemaphore
{
};

class CString
{
public:
   CString();
   CString(const CString& ref);
   CString(QString str);
   CString(LPCSTR str);
   CString(LPCWSTR str);
   virtual ~CString();

   void UpdateScratch();

   CString& Append(LPCSTR str);
   CString& Append(LPWSTR str);
   void AppendFormat(LPCTSTR fmt, ...);
   void AppendFormatV(LPCTSTR fmt, va_list ap);
   void Format(LPCTSTR fmt, ...);
   void FormatV(LPCTSTR fmt, va_list ap);

   CString& operator=(LPSTR str);
   CString& operator+=(LPSTR str);
   CString& operator=(LPWSTR str);
   CString& operator+=(LPWSTR str);
   CString& operator=(LPCSTR str);
   CString& operator+=(LPCSTR str);
   CString& operator=(LPCWSTR str);
   CString& operator+=(LPCWSTR str);
   CString& operator=(QString str);
   CString& operator+=(QString str);
   CString& operator=(CString str);
   CString& operator+=(CString str);
   bool operator==(const CString& str) const;
   //operator const char*() const;
   operator const QString&() const;
   operator LPCTSTR() const;

   void Empty();
   LPCTSTR GetString() const;
   LPCTSTR GetBuffer() const;
   CString Left( int nCount ) const;
   CString Right( int nCount ) const;
   int GetLength() const;
   int CompareNoCase( LPCTSTR lpsz ) const;
   TCHAR GetAt( int nIndex ) const;
   
private:
   QString _qstr;
   QByteArray _qstrn;
};

class CStringArray
{
public:
   CString GetAt(int idx) { return _qlist.at(idx); }
   void SetAt(int idx, CString str) { _qlist.replace(idx,str); }
private:
   QList<CString> _qlist;
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
   CPoint(int _x, int _y)
   {
      x = _x;
      y = _y;
   }
   void SetPoint(int _x, int _y)
   {
      x = _x;
      y = _y;
   }
   bool operator==(CPoint p)
   {
      return ((p.x == x) && (p.y == y));
   }
};

class CRect : public tagRECT
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
   int Width() const { return right-left; }
   int Height() const { return bottom-top; }
   void MoveToXY(
      int x,
      int y 
   );
   void MoveToY(
         int y
   );
   void MoveToXY(
      POINT point 
   );   
   void DeflateRect( 
      int x, 
      int y  
   );
   void DeflateRect( 
      SIZE size  
   );
   void DeflateRect( 
      LPCRECT lpRect  
   );
   void DeflateRect( 
      int l, 
      int t, 
      int r, 
      int b  
   );
   operator LPRECT() const
   {
      return (RECT*)this;
   }
   operator LPCRECT() const
   {
      return (const RECT*)this;
   }
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
   BOOL CreatePen(
      int nPenStyle,
      int nWidth,
      COLORREF crColor 
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
   
   QPainter* painter() { return _qpainter; }
   
   virtual ~CDC();
   BOOL DrawEdge(
      LPRECT lpRect,
      UINT nEdge,
      UINT nFlags 
   );
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
   COLORREF GetPixel(
      int x,
      int y 
   ) const;
   COLORREF GetPixel(
      POINT point 
   ) const;
   void Draw3dRect( LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight )
   {
      Draw3dRect(lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top,clrTopLeft,clrBottomRight);
   }
   void Draw3dRect( int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight );
   virtual int DrawText(
      LPCTSTR lpszString,
      int nCount,
      LPRECT lpRect,
      UINT nFormat 
   );
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

class CWnd;
class CScrollBar : public QScrollBar
{
public:
   CScrollBar() {}
   CScrollBar(Qt::Orientation o) : QScrollBar(o) {}
   BOOL SetScrollInfo(
      LPSCROLLINFO lpScrollInfo,
      BOOL bRedraw = TRUE 
   );
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   int SetScrollPos(
      int nPos,
      BOOL bRedraw = TRUE 
   );
   void SetScrollRange(
      int nMinPos,
      int nMaxPos,
      BOOL bRedraw = TRUE 
   );
   void ShowScrollBar(
      BOOL bShow = TRUE 
   );
   BOOL EnableScrollBar(
      UINT nArrowFlags = ESB_ENABLE_BOTH 
   );
};

class CDataExchange
{
};

class CFrameWnd;
class CWnd : public QWidget
{
   Q_OBJECT
public:
   CWnd(QWidget* parent=0);
   virtual ~CWnd();

   virtual BOOL CreateEx(
      DWORD dwExStyle,
      LPCTSTR lpszClassName,
      LPCTSTR lpszWindowName,
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID,
      LPVOID lpParam = NULL
   );
   virtual BOOL PreTranslateMessage(
      MSG* pMsg 
   ) {}
   virtual CScrollBar* GetScrollBarCtrl(
      int nBar 
   ) const;
   BOOL SetScrollInfo(
      int nBar,
      LPSCROLLINFO lpScrollInfo,
      BOOL bRedraw = TRUE 
   );
   void SetScrollRange(
      int nBar,
      int nMinPos,
      int nMaxPos,
      BOOL bRedraw = TRUE 
   );
   int SetScrollPos(
      int nBar,
      int nPos,
      BOOL bRedraw = TRUE 
   );
   virtual afx_msg int OnCreate(
      LPCREATESTRUCT lpCreateStruct 
   );
   void OnMouseMove(UINT,CPoint) {}
   void OnNcMouseMove(UINT nHitTest, CPoint point) {}
   void OnLButtonDblClk(UINT,CPoint) {}
   void OnLButtonDown(UINT,CPoint) {}
   void OnLButtonUp(UINT,CPoint) {}
   void OnRButtonDown(UINT,CPoint) {}
   void OnRButtonUp(UINT,CPoint) {}
   BOOL OnMouseWheel(UINT,UINT,CPoint) { return TRUE; }
   void OnSize(UINT nType, int cx, int cy) {}
   UINT SetTimer(UINT id, UINT interval, void*);
   void KillTimer(UINT id);
   void OnTimer(UINT timerId) {}
   void OnKeyDown(UINT,UINT,UINT) {}
   void OnSetFocus(CWnd*) {}
   void OnKillFocus(CWnd*) {}
   void OnVScroll(UINT,UINT,CScrollBar*) {}
   void OnHScroll(UINT,UINT,CScrollBar*) {}
   void OnUpdate(CWnd* p=0,UINT hint=0,CObject* o=0) { update(); }
   void Invalidate(BOOL bErase = TRUE) { /*update();*/ }
   void RedrawWindow(LPCRECT rect=0,CRgn* rgn=0,UINT f=0) { update(); }
   CWnd* SetFocus() { CWnd* pWnd = focusWnd; setFocus(); return pWnd; }
   CWnd* GetFocus() { return focusWnd; } 
   void SetCapture(CWnd* p=0) { /* DON'T DO THIS grabMouse(); */ }
   void ReleaseCapture() { /* DON'T DO THIS releaseMouse(); */ }
   CFrameWnd* GetParentFrame( ) const { return m_pFrameWnd; }
   void MoveWindow(int x,int y,int cx, int cy) { setFixedWidth(cx); }
   CDC* GetDC() { CDC* pDC = new CDC(); pDC->attach(this); return pDC; }
   void ReleaseDC(CDC* pDC) { pDC->detach(); delete pDC; }
   void ShowWindow(int code);
   virtual BOOL DestroyWindow( ) { return TRUE; }
   void UpdateWindow( ) { repaint(); }
   virtual BOOL PostMessage(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0 
   );
   virtual void DoDataExchange(
      CDataExchange* pDX 
   ) {}   
   CWnd* GetParent() { return (CWnd*)m_pFrameWnd; }
   void GetWindowRect(
      LPRECT lpRect 
   ) const;
   void GetClientRect(
      LPRECT lpRect 
   ) const;
   
   // This method only for Qt glue
   UINT_PTR mfcTimerId(int qtTimerId) { return qtToMfcTimer.value(qtTimerId); }
   // These methods are only to be used in CDocTemplate initialization...
   void privateSetParentFrame(CFrameWnd* pFrameWnd) { m_pFrameWnd = pFrameWnd; }
protected:
   QMap<UINT_PTR,int> mfcToQtTimer;
   QMap<int,UINT_PTR> qtToMfcTimer;
   CFrameWnd* m_pFrameWnd;
   static CWnd* focusWnd;
   CScrollBar* verticalScrollBar;
   CScrollBar* horizontalScrollBar;
};

class CView;
class CDocument;
class CFrameWnd : public CWnd
{
public:
   CFrameWnd() : CWnd(), m_pView(NULL), m_pDocument(NULL) {}
   virtual ~CFrameWnd() {}
   virtual void SetMessageText(LPCTSTR fmt,...) { qDebug("SetMessageText"); }
   CView* GetActiveView( ) const { return m_pView; } // Only one view for SDI
   virtual CDocument* GetActiveDocument( ) { return m_pDocument; }   
   
   // These methods are only to be used in CDocTemplate initialization...
   virtual void privateSetActiveView(CView* pView) { m_pView = pView; }
   virtual void privateSetActiveDocument(CDocument* pDocument) { m_pDocument = pDocument; }
   
protected:
   CView* m_pView;
   CDocument* m_pDocument;
};

class CDocTemplate;
class CDocument
{
public:
   CDocument() : m_pDocTemplate(NULL) {}
   virtual BOOL OnNewDocument() { DeleteContents(); return TRUE; }
   virtual BOOL OnSaveDocument(LPCTSTR lpszPathName) { return TRUE; }
   virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) { return TRUE; }
   virtual void OnCloseDocument() {}
   virtual void DeleteContents() {}
   virtual void SetModifiedFlag(BOOL bModified = 1) {}
   virtual void OnFileSave() {}
   virtual POSITION GetFirstViewPosition() const { return (POSITION)-1; }
   virtual CView* GetNextView(POSITION pos) const { return m_pViews.at((int)pos); }
   CDocTemplate* GetDocTemplate() const { return m_pDocTemplate; }

   // These methods are only to be used in CDocTemplate initialization...
   virtual void privateSetDocTemplate(CDocTemplate* pDocTemplate) { m_pDocTemplate = pDocTemplate; }
   virtual void privateAddView(CView* pView) { m_pViews.append(pView); }
   
protected:
   CDocTemplate* m_pDocTemplate;
   QList<CView*> m_pViews;
};

class CView : public CWnd
{
public:
   CView(QWidget* parent) : CWnd(parent), m_pDocument(NULL) {}
   virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
   virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {}  
   virtual void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
   virtual void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {}  
   CDocument* GetDocument() const { return m_pDocument; }
   
   // These methods are only to be used in CDocTemplate initialization...
   void privateSetDocument(CDocument* pDocument) { m_pDocument = pDocument; }

protected:
   CDocument* m_pDocument;
};

class CMenu : public QMenu
{
public:
   BOOL CreatePopupMenu() { return TRUE; }
   BOOL AppendMenu(
      UINT nFlags,
      UINT_PTR nIDNewItem = 0,
      LPCTSTR lpszNewItem = NULL 
   );
   UINT CheckMenuItem(
      UINT nIDCheckItem,
      UINT nCheck 
   );
   BOOL TrackPopupMenu(
      UINT nFlags,
      int x,
      int y,
      CWnd* pWnd,
      LPCRECT lpRect = 0
   );
private:
   QMap<UINT_PTR,QAction*> mfcToQtMenu;
};

class CDialog : public CWnd
{
public:
   CDialog(int dlgID,CWnd* parent) : CWnd(parent) {}
   BOOL OnInitDialog() { return TRUE; }
};

class CTabCtrl : public QTabWidget, public CWnd
{   
};

class CListCtrl : public QListWidget
{
};

class CComboBox : public QComboBox
{
public:
   void ResetContent();
   int AddString(CString& text);
   void SetCurSel(int sel);
};

class CWinThread : public QThread
{
   Q_OBJECT
public:
   CWinThread();
   virtual ~CWinThread();
   BOOL CreateThread(
      DWORD dwCreateFlags = 0,
      UINT nStackSize = 0,
      LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL 
   );
   BOOL PostThreadMessage(
      UINT message ,
      WPARAM wParam,
      LPARAM lParam 
         );
   virtual BOOL InitInstance() { return FALSE; }
   virtual BOOL ExitInstance() { return FALSE; }
signals:
   void postThreadMessage(unsigned int m,unsigned int w,unsigned int l);
public slots:
   void recvThreadMessage(unsigned int m,unsigned int w,unsigned int l) { qDebug("CWinThread::recvThreadMessage"); }
};

class CDocTemplate
{
public:
   CDocTemplate(UINT f,CDocument* pDoc,CFrameWnd* pFrameWnd,CView* pView);
   virtual CDocument* OpenDocumentFile(
      LPCTSTR lpszPathName,
      BOOL bMakeVisible = TRUE 
   ) = 0;
   CDocument* m_pDoc;
   CView*     m_pView;
   CFrameWnd* m_pFrameWnd;
};

class CSingleDocTemplate : public CDocTemplate
{
public:
   CSingleDocTemplate(UINT f,CDocument* pDoc,CFrameWnd* pFrameWnd,CView* pView);
   virtual CDocument* OpenDocumentFile(
      LPCTSTR lpszPathName,
      BOOL bMakeVisible = TRUE 
   );
};

class CWinApp : public CWinThread
{
public:
   CWinApp() : m_pMainWnd(NULL), m_pDocTemplate(NULL) {}
   void AddDocTemplate(CDocTemplate* pDocTemplate) { m_pDocTemplate = pDocTemplate; }
   CDocTemplate* GetDocTemplate() const { return m_pDocTemplate; }
   virtual BOOL InitInstance();
   CFrameWnd* m_pMainWnd;
   
protected:
   CDocTemplate* m_pDocTemplate;
};

int StretchDIBits(
  CDC& dc,
  int XDest,
  int YDest,
  int nDestWidth,
  int nDestHeight,
  int XSrc,
  int YSrc,
  int nSrcWidth,
  int nSrcHeight,
  const VOID *lpBits,
  const BITMAPINFO *lpBitsInfo,
  UINT iUsage,
  DWORD dwRop
);

#endif // CQTMFC_H
