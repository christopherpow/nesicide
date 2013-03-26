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
#include <QSize>
#include <QStatusBar>
#include <QPixmap>
#include <QFont>
#include <QRegion>
#include <QFrame>
#include <QAbstractButton>
#include <QRadioButton>
#include <QComboBox>
#include <QProgressBar>
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
#include <QListWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QFileDialog>

#ifndef QT_NO_DEBUG
//#define _DEBUG
#endif

// Define resources here that are "hidden under the hood" of MFC...
enum
{
   __UNDER_THE_HOOD_START = 0x8000000,
   
   IDC_STATIC,

   AFX_IDW_STATUS_BAR,
   ID_SEPARATOR,
   
//   STRINGTABLE 
//   BEGIN
   AFX_IDS_APP_TITLE       ,
   AFX_IDS_IDLEMESSAGE     ,
   AFX_IDS_HELPMODEMESSAGE,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_INDICATOR_EXT        ,
   ID_INDICATOR_CAPS       ,
   ID_INDICATOR_NUM        ,
   ID_INDICATOR_SCRL       ,
   ID_INDICATOR_OVR        ,
   ID_INDICATOR_REC        ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_FILE_NEW             ,
   ID_FILE_OPEN            ,
   ID_FILE_CLOSE           ,
   ID_FILE_SAVE            ,
   ID_FILE_SAVE_AS         ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_APP_ABOUT            ,
   ID_APP_EXIT             ,
   ID_HELP_INDEX           ,
   ID_HELP_FINDER          ,
   ID_HELP_USING           ,
   ID_CONTEXT_HELP         ,
   ID_HELP                 ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_FILE_MRU_FILE1       ,
   ID_FILE_MRU_FILE2       ,
   ID_FILE_MRU_FILE3       ,
   ID_FILE_MRU_FILE4       ,
   ID_FILE_MRU_FILE5       ,
   ID_FILE_MRU_FILE6       ,
   ID_FILE_MRU_FILE7       ,
   ID_FILE_MRU_FILE8       ,
   ID_FILE_MRU_FILE9       ,
   ID_FILE_MRU_FILE10      ,
   ID_FILE_MRU_FILE11      ,
   ID_FILE_MRU_FILE12      ,
   ID_FILE_MRU_FILE13      ,
   ID_FILE_MRU_FILE14      ,
   ID_FILE_MRU_FILE15      ,
   ID_FILE_MRU_FILE16      ,
//   END

//   STRINGTABLE 
//   BEGIN
   ID_NEXT_PANE            ,
   ID_PREV_PANE            ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_WINDOW_SPLIT         ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_EDIT_CLEAR           ,
   ID_EDIT_CLEAR_ALL       ,
   ID_EDIT_COPY            ,
   ID_EDIT_CUT             ,
   ID_EDIT_FIND            ,
   ID_EDIT_PASTE           ,
   ID_EDIT_REPEAT          ,
   ID_EDIT_REPLACE         ,
   ID_EDIT_SELECT_ALL      ,
   ID_EDIT_UNDO            ,
   ID_EDIT_REDO            ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   ID_VIEW_TOOLBAR         ,
   ID_VIEW_STATUS_BAR      ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   AFX_IDS_SCSIZE          ,
   AFX_IDS_SCMOVE          ,
   AFX_IDS_SCMINIMIZE      ,
   AFX_IDS_SCMAXIMIZE      ,
   AFX_IDS_SCNEXTWINDOW    ,
   AFX_IDS_SCPREVWINDOW    ,
   AFX_IDS_SCCLOSE         ,
//   END
   
//   STRINGTABLE 
//   BEGIN
   AFX_IDS_SCRESTORE       ,
   AFX_IDS_SCTASKLIST      ,
//   END
};

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

#if UNICODE
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

typedef int* POSITION;

#define DECLARE_DYNCREATE(x) 
#define DECLARE_MESSAGE_MAP()
#define DECLARE_DYNAMIC(x)
#define IMPLEMENT_DYNAMIC(x,y)

#define RUNTIME_CLASS(x) new x

#define afx_msg

#define strcpy_s(d,l,s) wcsncpy(d,s,l)
#if UNICODE
#define _ttoi _wtoi
#define _tcslen wcslen
#else
#define _ttoi atoi
#define _tcslen strlen
#endif
#ifdef QT_NO_DEBUG
#define ASSERT(y)
#define ASSERT_VALID(y)
#else
#define ASSERT(y) { if (!(y)) { QString str; str.sprintf("ASSERT: %s(%d)",__FILE__,__LINE__); qDebug(str.toLatin1().constData()); } }
#define ASSERT_VALID(y) { if (!(y)) { QString str; str.sprintf("ASSERT: %s(%d)",__FILE__,__LINE__); qDebug(str.toLatin1().constData()); } }
#endif

#define AFXAPI

int AfxMessageBox(
   LPCTSTR lpszText,
   UINT nType = MB_OK,
   UINT nIDHelp = 0 
);
int AFXAPI AfxMessageBox(
   UINT nIDPrompt,
   UINT nType = MB_OK,
   UINT nIDHelp = (UINT) -1 
);

size_t strlen(const wchar_t* str);

int _tstoi(TCHAR* str);

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
//  HWND hWndNewOwner = 0
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

class CDumpContext
{
};

class CObject
{
public:
   CObject() {}
   virtual ~CObject() {}
   virtual void DeleteObject() {}
};

class CCmdTarget : public CObject
{
};

#define AFX_MSG_CALL 
typedef struct
{
//   void( AFX_MSG_CALL CCmdTarget::* )( void ) 	pmf;
//   CCmdTarget* 	pTarget;
} AFX_CMDHANDLERINFO;

class CSyncObject
{
public:
   virtual BOOL Lock(
      DWORD dwTimeout = INFINITE 
   ) { return TRUE; }
   virtual BOOL Unlock( ) = 0; 
   virtual BOOL Unlock(
      LONG lCount,
      LPLONG lpPrevCount = NULL 
   ) { return TRUE; }
};

class CCriticalSection : public CSyncObject
{
public:
   CCriticalSection();
   virtual ~CCriticalSection();
   BOOL Lock(
      DWORD dwTimeout = INFINITE 
   );
   BOOL Unlock( ); 
protected:
   QMutex* _qtd;
};

class CMutex : public CSyncObject
{
public:
   CMutex(
      BOOL bInitiallyOwn = FALSE,
      LPCTSTR lpszName = NULL,
      LPSECURITY_ATTRIBUTES lpsaAttribute = NULL 
   );
   virtual ~CMutex();
   BOOL Lock(
      DWORD dwTimeout = INFINITE 
   );
   BOOL Unlock( ); 
protected:
   QMutex* _qtd;
};

class CSemaphore : public CSyncObject
{
};

class CString;
bool operator==(const CString& s1, const LPCTSTR s2);

class CString
{
public:
   CString();
   CString(const CString& ref);
   CString(QString str);
   CString(LPCSTR str);
   CString(LPCWSTR str);
   virtual ~CString();
   
   BOOL LoadString( UINT nID );
   
   void UpdateScratch();

   CString& Append(LPCSTR str);
   CString& Append(LPWSTR str);
   void AppendFormat(LPCTSTR fmt, ...);
   void AppendFormatV(LPCTSTR fmt, va_list ap);
   void Format( UINT nFormatID, ... );
   void Format(LPCTSTR fmt, ...);
   void FormatV(LPCTSTR fmt, va_list ap);
   void Truncate(int length);
   int ReverseFind( TCHAR ch ) const;
 
   const CString& operator=(const CString& str);
   const CString& operator+=(const CString& str);
   const CString& operator=(LPSTR str);
   const CString& operator+=(LPSTR str);
   const CString& operator=(LPWSTR str);
   const CString& operator+=(LPWSTR str);
   const CString& operator=(LPCSTR str);
   const CString& operator+=(LPCSTR str);
   const CString& operator=(LPCWSTR str);
   const CString& operator+=(LPCWSTR str);
   const CString& operator=(QString str);
   const CString& operator+=(QString str);
   const CString& operator+(const CString& str);
   const CString& operator+(LPSTR str);
   const CString& operator+(LPWSTR str);
   const CString& operator+(LPCSTR str);
   const CString& operator+(LPCWSTR str);
   const CString& operator+(QString str);
   operator const QString&() const;
   operator LPCTSTR() const;

   void Empty();
   LPCTSTR GetString() const;
   LPTSTR GetBuffer() const;
   CString Left( int nCount ) const;
   CString Right( int nCount ) const;
   int GetLength() const;
   int CompareNoCase( LPCTSTR lpsz ) const;
   TCHAR GetAt( int nIndex ) const;
   
protected:
   QString _qstr;
   QByteArray _qstrn;
};

class CStringA : public CString
{
public:
   CStringA(CString str) { qDebug("WHAT TO DO WITH CSTRINGA??"); }
};

class CStringArray
{
public:
   CString GetAt(int idx) { return _qlist.at(idx); }
   void SetAt(int idx, CString str) { _qlist.replace(idx,str); }
private:
   QList<CString> _qlist;
};

class CException
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

class CFileException : public CException
{
};

class CFile : public CCmdTarget
{
public:
   enum
   {
      modeCreate = 0x01,
      modeRead = 0x02,
      modeWrite = 0x04,
      shareDenyWrite = 0x08
   };
   enum
   {
      begin = 1,
      current = 2,
      end = 3
   };
   CFile();
   CFile(
      LPCTSTR lpszFileName,
      UINT nOpenFlags 
   );
   virtual ~CFile();

   virtual BOOL Open(
      LPCTSTR lpszFileName,
      UINT nOpenFlags,
      CFileException* pError = NULL
   );
   virtual ULONGLONG GetLength( ) const;
   virtual void Write(
      const void* lpBuf,
      UINT nCount
   );
   virtual ULONGLONG GetPosition( ) const;
   virtual UINT Read(
      void* lpBuf,
      UINT nCount
   );
   virtual ULONGLONG Seek(
      LONGLONG lOff,
      UINT nFrom 
   );
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

class CSize : public tagSIZE
{
public:
   CSize( ) { cx = 0; cy = 0; } 
   CSize( 
      int initCX, 
      int initCY  
   ) { cx = initCX; cy = initCY; }
   CSize( 
      SIZE initSize  
   ) { cx = initSize.cx; cy = initSize.cy; }
   CSize( 
      POINT initPt  
   ) { cx = initPt.x; cy = initPt.y; }
   CSize( 
      DWORD dwSize  
   ) { cx = dwSize&0xFFFF; cy = (dwSize>>16); }
   CSize(QSize qSize) { cx = qSize.width(); cy = qSize.height(); }
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
   void InflateRect( 
      int x, 
      int y  
   );
   void InflateRect( 
      SIZE size  
   );
   void InflateRect( 
      LPCRECT lpRect  
   );
   void InflateRect( 
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
   operator QRect() const
   {
      return QRect(left,top,right-left,bottom-top);
   }
};

class CGdiObject : public CObject
{
public:
   CGdiObject() {}
   virtual ~CGdiObject() {}
   operator HGDIOBJ() const
   {
      return (void*)this;
   }
};

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
   operator HPEN() const
   {
      return (HPEN)this;
   }

private:
   QPen _qpen;
};

class CDC;
class CBitmap : public CGdiObject
{
   // Qt interfaces
public:
   CBitmap(QString resource);
   QPixmap* toQPixmap() { return _qpixmap; }
   
   // MFC interfaces
public:
   CBitmap();
   virtual ~CBitmap();
   BOOL LoadBitmap(
      UINT nIDResource 
   );
   BOOL CreateCompatibleBitmap(
      CDC* pDC,
      int nWidth,
      int nHeight 
   );
   CSize SetBitmapDimension(
      int nWidth,
      int nHeight 
   );
   operator QPixmap() const
   {
      return *_qpixmap;
   }
   operator HBITMAP() const
   {
      return (HBITMAP)this;
   }
private:
   QPixmap* _qpixmap;
   bool     _owned;
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
   operator HBRUSH() const
   {
      return (HBRUSH)this;
   }
private:
   QBrush _qbrush;
};

class CFont : public CGdiObject
{
public:
   CFont() {}
   virtual ~CFont() {}
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
   operator QFont() const
   {
      return _qfont;
   }
   operator HFONT() const
   {
      return (HFONT)this;
   }
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
   operator HRGN() const
   {
      return (HRGN)this;
   }
private:
   QRegion _qregion;
};

class CWnd;

class CDC : public CObject
{
public:
   CDC();
   CDC(CWnd* parent);
   virtual ~CDC();

   void attach();
   void attach(QWidget* parent);
   void detach();
   void flush();
   void doFlush(bool doIt) { _doFlush = doIt; }
   QPainter* painter() { return _qpainter; }
   QPixmap* pixmap() { return _qpixmap; }
   QSize pixmapSize() { return _bitmapSize; }
   QWidget* widget() { return _qwidget; }
   QPixmap* bitmap() { return _bitmap?_bitmap->toQPixmap():NULL; }
   
   BOOL CreateCompatibleDC(
      CDC* pDC 
   );
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
   
   HGDIOBJ SelectObject(
      HGDIOBJ obj
   );
   CPen* SelectObject(
      CPen* pPen 
   );
   CBrush* SelectObject(
      CBrush* pBrush 
   );
   virtual CFont* SelectObject(
      CFont* pFont 
   );
   CBitmap* SelectObject(
      CBitmap* pBitmap 
   );
   int SelectObject(
      CRgn* pRgn 
   );
   CGdiObject* SelectObject(
      CGdiObject* pObject
   );
   CObject* SelectObject(
      CObject* pObject
   );
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
   virtual CPoint SetViewportOrg( 
      int x, 
      int y  
   )
   {
      CPoint old = _viewportOrg;
      _viewportOrg.x = x;
      _viewportOrg.y = y;
      return old;
   }
   CPoint SetViewportOrg( 
      POINT point  
   )
   {
      return SetViewportOrg(point.x,point.y);
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
   
public:
   HDC         m_hDC;
   
private:
   CDC(CDC& orig);
   bool attached;
   bool _doFlush;
   QWidget*    _qwidget;
   QPixmap*    _qpixmap;
   QPainter*   _qpainter;
   CPen*       _pen;
   CBrush*     _brush;
   CFont*      _font;
   CBitmap*    _bitmap;
   QSize       _bitmapSize;
   CRgn*       _rgn;   
   CGdiObject* _gdiobject;
   CObject*    _object;
   CPoint      _lineOrg;
   QColor      _bkColor;
   int         _bkMode;
   QColor      _textColor;
   CPoint      _windowOrg;
   CPoint      _viewportOrg;
};

class CPaintDC : public CDC
{
public:
   CPaintDC(CWnd* parent) : CDC(parent) {}
};

class CDataExchange
{
};

class QtUIElement
{
public:
   virtual void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   ) {}
   virtual UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const { return 0; }
   virtual void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   ) {}
   virtual int GetDlgItemText(
      int nID,
      CString& rString 
   ) const { return 0; }
   virtual int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const { return 0; }
   virtual void CheckDlgButton( 
      int nIDButton, 
      UINT nCheck  
   ) {}
   virtual UINT IsDlgButtonChecked( 
      int nIDButton
   ) const { return 0; }
};

class CFrameWnd;
class CScrollBar;

class CWnd : public QWidget, public CCmdTarget, public QtUIElement
{
   Q_OBJECT
   // MFC interfaces
public:
   CWnd(CWnd* parent=0);
   virtual ~CWnd();

   BOOL IsWindowVisible( ) const;
   virtual BOOL EnableWindow(
      BOOL bEnable = TRUE 
   );
   virtual BOOL PreCreateWindow(
      CREATESTRUCT& cs 
   ) { return TRUE; }
   enum
   {
      reposDefault,
      reposQuery,
      reposExtra
   };
   void UpdateDialogControls(
      CCmdTarget* pTarget,
      BOOL bDisableIfNoHndler 
   );
   void RepositionBars(
      UINT nIDFirst,
      UINT nIDLast,
      UINT nIDLeftOver,
      UINT nFlag = reposDefault,
      LPRECT lpRectParam = NULL,
      LPCRECT lpRectClient = NULL,
      BOOL bStretch = TRUE 
   );
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
   ) { return FALSE; }
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
   void OnNcLButtonUp(
      UINT nHitTest,
      CPoint point 
   ) {}   
   BOOL OnEraseBkgnd(
      CDC* pDC 
   ) { return TRUE; }
   HBRUSH OnCtlColor(
      CDC* pDC,
      CWnd* pWnd,
      UINT nCtlColor 
   ) { return (HBRUSH)NULL; }
   afx_msg void OnPaint( ) {}
   virtual BOOL OnCmdMsg(
      UINT nID,
      int nCode,
      void* pExtra,
      AFX_CMDHANDLERINFO* pHandlerInfo 
   ) { return FALSE; }
   virtual BOOL OnNotify( 
      WPARAM wParam, 
      LPARAM lParam, 
      LRESULT* pResult  
   ) { return FALSE; }
   void OnLButtonDblClk(UINT,CPoint) {}
   void OnLButtonDown(UINT,CPoint) {}
   void OnLButtonUp(UINT,CPoint) {}
   void OnMButtonDblClk(UINT,CPoint) {}
   void OnMButtonDown(UINT,CPoint) {}
   void OnMButtonUp(UINT,CPoint) {}
   void OnRButtonDblClk(UINT,CPoint) {}
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
   void OnUpdate(CWnd* p=0,UINT hint=0,CObject* o=0) { _qt->update(); }
   void Invalidate(BOOL bErase = TRUE) { /*update();*/ }
   void RedrawWindow(LPCRECT rect=0,CRgn* rgn=0,UINT f=0) { _qt->update(); }
   CWnd* SetFocus() { CWnd* pWnd = focusWnd; _qt->setFocus(); return pWnd; }
   CWnd* GetFocus() { return focusWnd; } 
   void SetCapture(CWnd* p=0) { /* DON'T DO THIS grabMouse(); */ }
   void ReleaseCapture() { /* DON'T DO THIS releaseMouse(); */ }
   CFrameWnd* GetParentFrame( ) const { return m_pFrameWnd; }
   void MoveWindow(
      LPCRECT lpRect,
         BOOL bRepaint = TRUE 
   );
   void MoveWindow(int x,int y,int cx, int cy);
   CDC* GetDC();
   void ReleaseDC(CDC* pDC);
   void ShowWindow(int code);
   void UpdateWindow( ) { _qt->update(); }
   virtual BOOL PostMessage(
      UINT message,
      WPARAM wParam = 0,
      LPARAM lParam = 0 
   );
   virtual void DoDataExchange(
      CDataExchange* pDX 
   ) {}   
   CWnd* GetParent() { return m_pParentWnd?(CWnd*)m_pParentWnd:(CWnd*)m_pFrameWnd; }
   void SetParent(CWnd* parent) { m_pParentWnd = parent; }
   void GetWindowText(
      CString& rString 
   ) const;
   void SetWindowText(
      LPCTSTR lpszString 
   );
   void GetWindowRect(
      LPRECT lpRect 
   ) const;
   void GetClientRect(
      LPRECT lpRect 
   ) const;
   CWnd* GetDlgItem(
      int nID 
   ) const;
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
   void CheckDlgButton( 
      int nIDButton, 
      UINT nCheck  
   );
   UINT IsDlgButtonChecked( 
      int nIDButton
   ) const;
   BOOL SubclassDlgItem(
      UINT nID,
      CWnd* pParent 
   );
   virtual BOOL DestroyWindow( ) { _qt->close(); return TRUE; }
   virtual void PostNcDestroy( ) {}  
   
   // This method only for Qt glue
   UINT_PTR mfcTimerId(int qtTimerId) { return qtToMfcTimer.value(qtTimerId); }
   // These methods are only to be used in CDocTemplate initialization...
   void privateSetParentFrame(CFrameWnd* pFrameWnd) { m_pFrameWnd = pFrameWnd; }
   
   // MFC-to-Qt conversions
protected:
   QMap<UINT_PTR,int> mfcToQtTimer;
   QMap<int,UINT_PTR> qtToMfcTimer;
   QMap<int,CWnd*> mfcToQtWidget;
   static CFrameWnd* m_pFrameWnd;
   CWnd* m_pParentWnd;
   static CWnd* focusWnd;
   CScrollBar* mfcVerticalScrollBar;
   CScrollBar* mfcHorizontalScrollBar;
   CDC* myDC;

   // Qt interfaces
public:
   void subclassWidget(int nID,CWnd* widget);
   void setParent(QWidget *parent) { _qt->setParent(parent); }
   void setParent(QWidget *parent, Qt::WindowFlags f) { _qt->setParent(parent,f); }   
   void setGeometry(const QRect & rect) { _qt->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored); _qt->setGeometry(rect); }
   void setGeometry(int x, int y, int w, int h) { _qt->setGeometry(x,y,w,h); }
   const QRect &	geometry () const { return _qt->geometry(); }
   void setContentsMargins(int left, int top, int right, int bottom) { _qt->setContentsMargins(left,top,right,bottom); }
   void setContentsMargins(const QMargins &margins) { _qt->setContentsMargins(margins); }
   void setFocusPolicy(Qt::FocusPolicy policy) { _qt->setFocusPolicy(policy); }
   void setFixedSize(int w, int h) { _qt->setFixedSize(w,h); }
   virtual void setVisible(bool visible) { _qt->setVisible(visible); }
   virtual void setEnabled(bool enabled) { _qt->setEnabled(enabled); }
   QRect rect() const { return _qt->rect(); }
   virtual QWidget* toQWidget() { return _qt; }
public slots:
   void update() { _qt->update(); }
   void repaint() { _qt->repaint(); }
   void setFocus() { _qt->setFocus(); }
   void setFocus(Qt::FocusReason reason) { _qt->setFocus(reason); }
   bool eventFilter(QObject *object, QEvent *event);
protected:
   QWidget* _qt;
public:
   HWND m_hWnd;
};

class CView;
class CDocument;
class CFrameWnd : public CWnd
{
   // Qt interfaces
   
   // MFC interfaces
public:
   CFrameWnd(CWnd* parent = 0);
   virtual ~CFrameWnd();
   virtual void SetMessageText(LPCTSTR fmt,...) { qDebug("SetMessageText"); }
   CView* GetActiveView( ) const { return m_pView; } // Only one view for SDI
   virtual CDocument* GetActiveDocument( ) { return m_pDocument; }   
   virtual void RecalcLayout(
      BOOL bNotify = TRUE 
   );   
   // These methods are only to be used in CDocTemplate initialization...
   virtual void privateSetActiveView(CView* pView) { m_pView = pView; }
   virtual void privateSetActiveDocument(CDocument* pDocument) { m_pDocument = pDocument; }
   
protected:
   CView* m_pView;
   CDocument* m_pDocument;
};

class CDocTemplate;
class CDocument : public CCmdTarget
{
public:
   CDocument() : m_pDocTemplate(NULL) {}
   void AssertValid() const {}
   void Dump(CDumpContext& dc) const {}
   virtual BOOL OnNewDocument() { DeleteContents(); return TRUE; }
   virtual BOOL OnSaveDocument(LPCTSTR lpszPathName) { return TRUE; }
   virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) { return TRUE; }
   virtual void OnCloseDocument() {}
   virtual void DeleteContents() {}
   virtual void SetModifiedFlag(BOOL bModified = 1) {}
   virtual void OnFileSave() {}
   virtual POSITION GetFirstViewPosition() const; 
   virtual CView* GetNextView(POSITION pos) const;
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
   CView(CWnd* parent);
   virtual ~CView();
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

class CMenu : public CCmdTarget
{
   // Qt interfaces
public:
   QMenu* toQMenu() { return _qtd; }
   
   // MFC interface
public:
   CMenu();
   BOOL CreatePopupMenu();
   BOOL LoadMenu(
      UINT nIDResource 
   );
   CMenu* GetSubMenu(
      int nPos 
   ) const;
   BOOL AppendMenu(
      UINT nFlags,
      UINT_PTR nIDNewItem = 0,
      LPCTSTR lpszNewItem = NULL 
   );
   BOOL AppendMenu(
      UINT nFlags,
      UINT_PTR nIDNewItem = 0,
      char* lpszNewItem = NULL 
   );
   BOOL SetDefaultItem(
      UINT uItem,
      BOOL fByPos = FALSE 
   );
   UINT CheckMenuItem(
      UINT nIDCheckItem,
      UINT nCheck 
   );
   UINT EnableMenuItem(
      UINT nIDEnableItem,
      UINT nEnable 
   );
   BOOL TrackPopupMenu(
      UINT nFlags,
      int x,
      int y,
      CWnd* pWnd,
      LPCRECT lpRect = 0
   );
   BOOL DestroyMenu( );
private:
   QMenu* _qtd;
   QMap<UINT_PTR,QAction*> mfcToQtMenu;
   QMap<QAction*,UINT_PTR> qtToMfcMenu;
};

class CDialog : public CWnd
{
   // Qt interfaces
public:
   QDialog* _qtd;
   bool _inited;
protected:
   
   // MFC interfaces
public:
   CDialog(int dlgID,CWnd* parent);
   virtual ~CDialog();
   void EndDialog(
      int nResult 
   );
   virtual void OnOK( ) { _qtd->accept(); }
   virtual void OnCancel( ) { _qtd->reject(); }
   void ShowWindow(int code);
   virtual BOOL Create(
      UINT nIDTemplate,
      CWnd* pParentWnd = NULL 
         );
   virtual BOOL OnInitDialog() { return TRUE; }
   virtual INT_PTR DoModal();
   void MapDialogRect( 
      LPRECT lpRect  
   ) const;
};

class CCommonDialog : public CDialog
{
   // Qt interfaces
public:
protected:
   
public:
   explicit CCommonDialog(
      CWnd* pParentWnd 
   );
   virtual ~CCommonDialog();
};

class CFileDialog : public CCommonDialog
{
   // Qt interfaces
public:
   void setDefaultSuffix(const QString & suffix) { _qtd->setDefaultSuffix(suffix); }
   void selectFile(const QString & filename) { _qtd->selectFile(filename); }
   void setFilter(const QString& filter) { _qtd->setFilter(filter); }
   QStringList selectedFiles() const { return _qtd->selectedFiles(); }
protected:
   QFileDialog* _qtd;
   
   // MFC interfaces
public:
   explicit CFileDialog(
      BOOL bOpenFileDialog,
      LPCTSTR lpszDefExt = NULL,
      LPCTSTR lpszFileName = NULL,
      DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      LPCTSTR lpszFilter = NULL,
      CWnd* pParentWnd = NULL,
      DWORD dwSize = 0
   );
   virtual ~CFileDialog();
   INT_PTR DoModal();
   virtual void OnFileNameChange( ) {};
   CString GetFileExt( ) const;
   CString GetFileName( ) const;
   CString GetPathName( ) const;
   POSITION GetStartPosition( ) const;
   CString GetNextPathName(
      POSITION& pos 
   ) const;
   LPOPENFILENAME m_pOFN;
};

class CScrollBar : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   int sliderPosition() const { return _qtd->sliderPosition(); }
   void setMinimum(int minimum) { _qtd->setMinimum(minimum); }
   void setMaximum(int maximum) { _qtd->setMaximum(maximum); }
   void setValue(int value) { _qtd->setValue(value); }
   void setPageStep(int pageStep) { _qtd->setPageStep(pageStep); }
protected:
   QScrollBar* _qtd;
   Qt::Orientation _orient;
signals:
   void actionTriggered(int action);
   
   // MFC interfaces
public:
   CScrollBar(CWnd* parent = 0);
   virtual ~CScrollBar();
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

class CSpinButtonCtrl; // CP: for buddy...
class CEdit : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setBuddy(CSpinButtonCtrl* buddy) { _buddy = buddy; }
   void setText(QString text) { _qtd->setText(text); }
protected:
   QLineEdit* _qtd;
   CSpinButtonCtrl* _buddy;
signals:
   void textChanged(QString);
   
   // MFC interfaces
public:
   CEdit(CWnd* parent = 0);
   virtual ~CEdit();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   BOOL EnableWindow(BOOL bEnable);
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

class CButton : public CWnd
{
   Q_OBJECT
   // Qt interfaces
protected:
   QAbstractButton* _qtd;
   QPushButton* _qtd_push;
   QRadioButton* _qtd_radio;
   QCheckBox* _qtd_check;
signals:
   void clicked();

   // MFC interfaces
public:
   CButton(CWnd* parent = 0);
   virtual ~CButton();
   virtual BOOL Create(
      LPCTSTR lpszCaption,
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
   void CheckDlgButton( 
      int nIDButton, 
      UINT nCheck  
   );
   UINT IsDlgButtonChecked( 
      int nIDButton
   ) const;
};

// From mingw/include/commctrl.h
#define TBS_AUTOTICKS	1
#define TBS_VERT	2
#define TBS_HORZ	0
#define TBS_TOP	4
#define TBS_BOTTOM	0
#define TBS_LEFT	4
#define TBS_RIGHT	0
#define TBS_BOTH	8
#define TBS_NOTICKS	16
#define TBS_ENABLESELRANGE	32
#define TBS_FIXEDLENGTH	64
#define TBS_NOTHUMB	128

class CSliderCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
protected:
   QSlider* _qtd;
signals:
   void valueChanged(int);
      
   // MFC interfaces
public:
   CSliderCtrl(CWnd* parent = 0);
   virtual ~CSliderCtrl();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   void SetRange(
      short nLower,
      short nUpper 
   );   
   void SetRangeMax(
      int nMax,
      BOOL bRedraw = FALSE 
   );   
   void SetPos(
      int nPos 
   );
   int GetPos( ) const;
   void SetTicFreq(
      int nFreq 
   );
};

class CProgressCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setOrientation(Qt::Orientation orient) { _qtd->setOrientation(orient); }
   void setInvertedAppearance(bool inverted) { _qtd->setInvertedAppearance(inverted); }
protected:
   QProgressBar* _qtd;
      
   // MFC interfaces
public:
   CProgressCtrl(CWnd* parent = 0);
   virtual ~CProgressCtrl();
   void SetRange(
      short nLower,
      short nUpper 
   );   
   void SetPos(
      int nPos 
   );
   int GetPos( ) const;
};

enum
{
   UDS_SETBUDDYINT = 0x1,
   UDS_ALIGNRIGHT = 0x2,
   UDS_AUTOBUDDY = 0x4,
   UDS_ARROWKEYS = 0x8,
   UDS_NOTHOUSANDS = 0x10
};

class CSpinButtonCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setBuddy(CEdit* buddy) { _buddy = buddy; }
protected:
   QSpinBox* _qtd;
   CEdit* _buddy;
signals:
   void valueChanged(int);
   
   // MFC interfaces
public:
   CSpinButtonCtrl(CWnd* parent = 0);
   virtual ~CSpinButtonCtrl();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   int SetPos(
      int nPos 
   );
   int GetPos( ) const; 
   void SetRange(
      short nLower,
      short nUpper 
   );   
};

class CComboBox : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
protected:
   QComboBox* _qtd;
signals:
   void currentIndexChanged(int index);
   
   // MFC interfaces
public:
   CComboBox(CWnd* parent = 0);
   virtual ~CComboBox();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   void ResetContent();
   int AddString(
      LPCTSTR lpszString 
   );
   void SetCurSel(int sel);
   int GetCurSel( ) const;
   int GetLBText(
      int nIndex,
      LPTSTR lpszText 
   ) const;
#if UNICODE
   int GetLBText(
      int nIndex,
      char* lpszText 
   ) const;
#endif
   void GetLBText(
      int nIndex,
      CString& rString 
   ) const;
   int SelectString(
      int nStartAfter,
      LPCTSTR lpszString 
   );
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

class CStatic : public CWnd
{
   // Qt interfaces
public:
   void setText(const QString & text) { _qtd->setText(text); }
protected:
   QLabel* _qtd;

   // MFC interfaces
public:
   CStatic(CWnd* parent = 0);
   virtual ~CStatic();
   virtual BOOL Create(
      LPCTSTR lpszText,
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID = 0xffff 
   );
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

// Qt hack for MFC BS_GROUPBOX style CButton override
class CGroupBox : public CWnd
{
   Q_OBJECT
   // Qt interfaces
protected:
   QGroupBox* _qtd;
signals:
   void clicked();

   // MFC interfaces
public:
   CGroupBox(CWnd* parent = 0);
   virtual ~CGroupBox();
   virtual BOOL Create(
      LPCTSTR lpszCaption,
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   void SetDlgItemInt(
      int nID,
      UINT nValue,
      BOOL bSigned = TRUE 
   );
   UINT GetDlgItemInt(
      int nID,
      BOOL* lpTrans = NULL,
      BOOL bSigned = TRUE 
   ) const;
   void SetDlgItemText(
      int nID,
      LPCTSTR lpszString 
   );
   int GetDlgItemText(
      int nID,
      CString& rString 
   ) const;
   int GetDlgItemText(
      int nID,
      LPTSTR lpStr,
      int nMaxCount 
   ) const;
};

class CTabCtrl : public CWnd
{  
   Q_OBJECT
   // Qt interfaces
public:
protected:
   QTabWidget* _qtd;
signals:
   void currentChanged(int);
   
   // MFC interfaces
public:
   CTabCtrl(CWnd* parent = 0);
   virtual ~CTabCtrl();
   LONG InsertItem(
     int nItem,
     LPCTSTR lpszItem 
   );
   BOOL DeleteAllItems( );
   int SetCurSel(
     int nItem 
   );
   int GetCurSel( ) const;
};

#define LVCFMT_LEFT 100

#define LVIS_SELECTED 1
#define LVIS_FOCUSED  2

// CP: No idea...need to find these in the windows headers.
#define LVIF_STATE 100 
#define LVNI_SELECTED 200

#define LVS_ICON	0
#define LVS_REPORT	1
#define LVS_SMALLICON	2
#define LVS_LIST	3
#define LVS_TYPEMASK	3
#define LVS_SINGLESEL	4
#define LVS_SHOWSELALWAYS	8
#define LVS_SORTASCENDING	16
#define LVS_SORTDESCENDING	32
#define LVS_SHAREIMAGELISTS	64
#define LVS_NOLABELWRAP	128
#define LVS_AUTOARRANGE	256
#define LVS_EDITLABELS	512
#define LVS_NOSCROLL	0x2000
#define LVS_TYPESTYLEMASK	0xfc00
#define LVS_ALIGNTOP	0
#define LVS_ALIGNLEFT	0x800
#define LVS_ALIGNMASK	0xc00
#define LVS_OWNERDRAWFIXED	0x400
#define LVS_NOCOLUMNHEADER	0x4000
#define LVS_NOSORTHEADER	0x8000

typedef struct tagNMLISTVIEW {
  NMHDR  hdr;
  int    iItem;
  int    iSubItem;
  UINT   uNewState;
  UINT   uOldState;
  UINT   uChanged;
  POINT  ptAction;
  LPARAM lParam;
} NMLISTVIEW, *LPNMLISTVIEW, NM_LISTVIEW;

typedef struct tagNMITEMACTIVATE {
  NMHDR  hdr;
  int    iItem;
  int    iSubItem;
  UINT   uNewState;
  UINT   uOldState;
  UINT   uChanged;
  POINT  ptAction;
  LPARAM lParam;
  UINT   uKeyFlags;
} NMITEMACTIVATE, *LPNMITEMACTIVATE;

class CListCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setSelectionMode(QAbstractItemView::SelectionMode mode) { _qtd->setSelectionMode(mode); }
   void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior) { _qtd->setSelectionBehavior(behavior); }
   QScrollBar* verticalScrollBar() const { return _qtd->verticalScrollBar(); }
   QScrollBar* horizontalScrollBar() const { return _qtd->horizontalScrollBar(); }
   QModelIndex currentIndex () const { return _qtd->currentIndex(); }
protected:
   QTableWidget* _qtd;
signals:
   void itemSelectionChanged();
   void cellClicked(int row, int column);
   void cellDoubleClicked(int row, int column);
   
   // MFC interfaces
public:
   CListCtrl(CWnd* parent = 0);
   virtual ~CListCtrl();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   BOOL DeleteAllItems( );
   BOOL DeleteItem(
      int nItem 
   );
   int InsertColumn(
      int nCol,
      LPCTSTR lpszColumnHeading,
      int nFormat = LVCFMT_LEFT,
      int nWidth = -1,
      int nSubItem = -1 
   );
   UINT GetSelectedCount( ) const;
   int GetSelectionMark( );
   int GetNextItem(
      int nItem,
      int nFlags 
   ) const;
   int GetItemText(
      int nItem,
      int nSubItem,
      LPTSTR lpszText,
      int nLen 
   ) const;
   CString GetItemText(
      int nItem,
      int nSubItem 
   ) const;
#if UNICODE
   int GetItemText(
      int nItem,
      int nSubItem,
      char* lpszText,
      int nLen 
   ) const;
#endif
   int InsertItem(
      int nItem,
      LPCTSTR lpszItem,
      int nImage 
   );
   int InsertItem(
      int nItem,
      LPCTSTR lpszItem 
   );
   int SetSelectionMark(
      int iIndex 
   );
   BOOL SetCheck(
      int nItem,
      BOOL fCheck = TRUE 
   );
   BOOL GetCheck(
      int nItem 
   ) const;
   BOOL SetItemText(
      int nItem,
      int nSubItem,
      LPCTSTR lpszText 
   );
   BOOL SetItemText(
      int nItem,
      int nSubItem,
      char* lpszText 
   );
   BOOL SetItemState(
      int nItem,
      UINT nState,
      UINT nMask 
   );
   int GetItemCount( ) const;
   DWORD_PTR GetItemData( 
      int nItem  
   ) const;
   BOOL SetItemData(
      int nItem,
         DWORD_PTR dwData 
   );
   BOOL EnsureVisible(
      int nItem,
      BOOL bPartialOK 
   );
};

class CListBox : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setSelectionMode(QAbstractItemView::SelectionMode mode) { _qtd->setSelectionMode(mode); }
   void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior) { _qtd->setSelectionBehavior(behavior); }
   QScrollBar* verticalScrollBar() const { return _qtd->verticalScrollBar(); }
   QScrollBar* horizontalScrollBar() const { return _qtd->horizontalScrollBar(); }
   QModelIndex currentIndex () const { return _qtd->currentIndex(); }
protected:
   QListWidget* _qtd;
signals:
   void itemSelectionChanged();
   void itemClicked(QListWidgetItem* lwi);
   void itemDoubleClicked(QListWidgetItem* lwi);
   
   // MFC interfaces
public:
   CListBox(CWnd* parent = 0);
   virtual ~CListBox();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
};

class CCheckListBox : public CListBox
{   
   // MFC interfaces
public:
   CCheckListBox(CWnd* parent = 0);
   virtual ~CCheckListBox();
};

typedef QTreeWidgetItem* HTREEITEM;

#define TVI_ROOT (HTREEITEM)0
#define TVI_LAST (HTREEITEM)0xffffffff

enum
{
   TVGN_CARET,
   TVGN_DROPHILITE,
   TVGN_FIRSTVISIBLE,
   TVGN_LASTVISIBLE,
   TVGN_NEXTVISIBLE,
   TVGN_PREVIOUSVISIBLE,
   TVGN_CHILD,
   TVGN_NEXT,
   TVGN_PARENT,
   TVGN_PREVIOUS,
   TVGN_ROOT
};

enum
{
   TVE_COLLAPSE,
   TVE_COLLAPSERESET,
   TVE_EXPAND,
   TVE_TOGGLE
};

#define TVS_HASBUTTONS	1
#define TVS_HASLINES	2
#define TVS_LINESATROOT	4
#define TVS_EDITLABELS	8
#define TVS_DISABLEDRAGDROP	16
#define TVS_SHOWSELALWAYS	32
#define TVS_CHECKBOXES 256
#define TVS_NOTOOLTIPS 128
#define TVS_RTLREADING 64
#define TVS_TRACKSELECT 512
#define TVS_FULLROWSELECT 4096
#define TVS_INFOTIP 2048
#define TVS_NONEVENHEIGHT 16384
#define TVS_NOSCROLL 8192
#define TVS_SINGLEEXPAND 1024
#define TVS_NOHSCROLL	0x8000

class CTreeCtrl : public CWnd
{
   Q_OBJECT
   // Qt interfaces
public:
   void setSelectionMode(QAbstractItemView::SelectionMode mode) { _qtd->setSelectionMode(mode); }
   void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior) { _qtd->setSelectionBehavior(behavior); }
   QScrollBar* verticalScrollBar() const { return _qtd->verticalScrollBar(); }
   QScrollBar* horizontalScrollBar() const { return _qtd->horizontalScrollBar(); }
   QModelIndex currentIndex () const { return _qtd->currentIndex(); }
protected:
   QTreeWidget* _qtd;
signals:
   void itemSelectionChanged();
   void itemClicked(QTreeWidgetItem* item, int column);
   void itemDoubleClicked(QTreeWidgetItem* item, int column);
   
   // MFC interfaces
public:
   CTreeCtrl(CWnd* parent = 0);
   virtual ~CTreeCtrl();
   virtual BOOL Create(
      DWORD dwStyle,
      const RECT& rect,
      CWnd* pParentWnd,
      UINT nID 
   );
   HTREEITEM InsertItem(
      LPCTSTR lpszItem,
      HTREEITEM hParent = TVI_ROOT,
      HTREEITEM hInsertAfter = TVI_LAST 
   );
   BOOL SortChildren(
      HTREEITEM hItem 
   );
   HTREEITEM GetRootItem( ) const;
   HTREEITEM GetNextItem(
      HTREEITEM hItem,
      UINT nCode 
   ) const;
   HTREEITEM GetSelectedItem( ) const;
   BOOL ItemHasChildren(
      HTREEITEM hItem 
   ) const;
   DWORD_PTR GetItemData(
      HTREEITEM hItem 
   ) const;
   BOOL SetItemData(
      HTREEITEM hItem,
      DWORD_PTR dwData 
   );   
   CString GetItemText(
      HTREEITEM hItem 
   ) const;
   BOOL DeleteItem(
      HTREEITEM hItem 
   );
   BOOL Expand(
      HTREEITEM hItem,
      UINT nCode 
   );
   HTREEITEM GetParentItem(
      HTREEITEM hItem 
   ) const;
};

class CWinThread : public QThread, public CCmdTarget
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

class CDocTemplate : public CCmdTarget
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
   HCURSOR LoadStandardCursor( 
      LPCTSTR lpszCursorName  
   ) const;
   virtual CWnd * GetMainWnd( ) { return m_pMainWnd; }
public:
   CFrameWnd* m_pMainWnd;
   
protected:
   CDocTemplate* m_pDocTemplate;
};

class CControlBar : public CWnd
{
};

#define CBRS_BOTTOM 0x1234

class CStatusBar : public CControlBar
{
   // Qt interfaces
public:
   QMap<int,CStatic*> panes() { return _panes; }
protected:
   QStatusBar* _qtd;
   QMap<int,CStatic*> _panes;
   
   // MFC interfaces
public:
   CStatusBar(CWnd* parent = 0);
   virtual ~CStatusBar();
   virtual BOOL Create(
      CWnd* pParentWnd,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
      UINT nID = AFX_IDW_STATUS_BAR 
   );
   BOOL SetIndicators(
      const UINT* lpIDArray,
      int nIDCount 
   );
   BOOL SetPaneText(
      int nIndex,
      LPCTSTR lpszNewText,
      BOOL bUpdate = TRUE 
   );   
};

class CCmdUI
{
public:
   void ContinueRouting( ) {}
   virtual void Enable(
      BOOL bOn = TRUE 
   ) { m_pOther->EnableWindow(bOn); }
   virtual void SetCheck(
      int nCheck = 1 
   ) { m_pMenu->CheckMenuItem(m_nID,nCheck); }
   virtual void SetRadio(
      BOOL bOn = TRUE 
   ) { m_pOther->CheckDlgButton(m_nID,bOn); }
   virtual void SetText(
      LPCTSTR lpszText 
   ) { m_pOther->SetDlgItemText(m_nID,lpszText); }
   UINT m_nID;
   UINT m_nIndex;
   CMenu* m_pMenu;
   CWnd* m_pOther;
   CMenu* m_pSubMenu;
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

CString qtMfcStringResource(int id);

CMenu qtMfcMenuResource(int id);

CBitmap qtMfcBitmapResource(int id);

#endif // CQTMFC_H
